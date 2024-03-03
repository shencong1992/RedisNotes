#include <stdio.h>
#include <stdlib.h>
#include "../../redis-5.0.0/src/sds.c"
#include "../../redis-5.0.0/src/zmalloc.c"


int clusterManagerSlotCompare(const void *slot1, const void *slot2) {
    const char **i1 = (const char **)slot1;
    const char **i2 = (const char **)slot2;
    return strcmp(*i1, *i2);
}


static sds clusterManagerGetConfigSignature(char *lines) {
    sds signature = NULL;
    int node_count = 0, i = 0, name_len = 0;
    char **node_configs = NULL;
    char *p, *line;
    while ((p = strstr(lines, "\n")) != NULL) {
        // 同clusterManagerGetConfigSignature()函数中的源码一致
        i = 0;
        *p = '\0';
        line = lines;
        lines = p + 1;
        char *nodename = NULL;
        int tot_size = 0;
        while ((p = strchr(line, ' ')) != NULL) {
            *p = '\0';
            char *token = line;
            line = p + 1;
            if (i == 0) {
                nodename = token;       // 获取第1个字段，即nodename 
                tot_size = (p - token);
                name_len = tot_size++; // Make room for ':' in tot_size
            } else if (i == 8) break;
            i++;
        }
        // 注意i != 8以及remaining == 0时，对应行的处理都将跳过
        if (i != 8) continue;   
        if (nodename == NULL) continue;
        int remaining = strlen(line);
        if (remaining == 0) continue;
        char **slots = NULL;
        int c = 0;
        while (remaining > 0) {    // 处理第9及其后面的字段，将保存到数组slots中
            p = strchr(line, ' ');
            if (p == NULL) p = line + remaining;
            int size = (p - line);
            remaining -= size;
            tot_size += size;
            char *slotsdef = line;
            *p = '\0';
            if (remaining) {
                line = p + 1;
                remaining--;
            } else line = p;
            if (slotsdef[0] != '[') {   // 要求该字段不能以字符'['开始
                c++;
                slots = zrealloc(slots, (c * sizeof(char *)));
                slots[c - 1] = slotsdef;
            }
        }
        if (c > 0) {
            if (c > 1)
                qsort(slots, c, sizeof(char *), clusterManagerSlotCompare);
            node_count++;
            node_configs =
                zrealloc(node_configs, (node_count * sizeof(char *)));
            /* Make room for '|' separators. */
            tot_size += (sizeof(char) * (c - 1));  // 给'|'留空间
            char *cfg = zmalloc((sizeof(char) * tot_size) + 1);  // 本行配置值的大小
            memcpy(cfg, nodename, name_len);     // 首先是nodename
            char *sp = cfg + name_len;             // 接着sp指向nodename后面的位置
            *(sp++) = ':';                            // 先加一个冒号
            for (i = 0; i < c; i++) {     // 将slots中的元素全部用'|'连接并添加到sp指向的位置
                if (i > 0) *(sp++) = '|';
                int slen = strlen(slots[i]);
                memcpy(sp, slots[i], slen);
                sp += slen;
            }
            *(sp++) = '\0';                            // 最后加上字符串终止符
            node_configs[node_count - 1] = cfg;  // 将本行的cfg保存到数组node_configs中
        }
        zfree(slots);
    }
    if (node_count > 0) {
        if (node_count > 1) {
            qsort(node_configs, node_count, sizeof(char *),
                  clusterManagerSlotCompare);
        }
        signature = sdsempty();
        for (i = 0; i < node_count; i++) {
            if (i > 0) signature = sdscatprintf(signature, "%c", '|');
            signature = sdscatfmt(signature, "%s", node_configs[i]);
        }
    }
cleanup:
    if (node_configs != NULL) {
        for (i = 0; i < node_count; i++) zfree(node_configs[i]);
        zfree(node_configs);
    }
    return signature;
}


int main() {
    char lines[2000];
    FILE *fp;
    fp = fopen("node_info", "r+");
    size_t nread = fread(lines, sizeof(char), sizeof(lines), fp);
    printf("%s\n", clusterManagerGetConfigSignature(lines));
    fclose(fp);
    return 0;
}
