// 演示函数4: 测试raxSeek()函数
#include <stdlib.h>
#include "../redis-5.0.0/src/rax.c"
#include "../redis-5.0.0/src/zmalloc.c"

// ctyun  footer  footxyz  header  heart  hello 
rax *gen_rax() {
    rax *r = raxNew();
    raxInsert(r, "hello", 5, "world", NULL);
    raxInsert(r, "footer", 6, "123", NULL);
    raxInsert(r, "footxyz", 7, "abc", NULL);
    raxInsert(r, "heart", 5, "haha", NULL);
    raxInsert(r, "header", 6, "kvrocks", NULL);
    raxInsert(r, "ctyun", 5, "cloud", NULL);

    // 打印rax结构
    raxShow(r);
    return r;
}

void print_iterator(raxIterator *it, char *tips) {
    char key[64];
    for(int i = 0; i < it->key_len; i++) {  // 打印迭代器所在raxNode节点代表的路径字符串
        key[i] = it->key[i];
    }
    key[it->key_len] = '\0';
    printf("%s: data=%s, size=%d, key=%s\n", tips, it->node->data, it->node->size, key);    
}

int main() {
    rax *r = gen_rax();

    raxIterator rt;
    raxStart(&rt, r);

    raxIterator *it = &rt;

    // ctyun
    raxSeek(it, "^", NULL, 0);
    print_iterator(it, "raxSeek() find least raxNode"); 

    // hello
    raxSeek(it, "$", NULL, 0);
    print_iterator(it, "raxSeek() find greatest raxNode"); 

    // header
    raxSeek(it, ">", "great", 5);
    print_iterator(it, "raxSeek() find greater then great");

    // ctyun 
    raxSeek(it, "<", "foota", 5);
    print_iterator(it, "raxSeek() find lower then footy");

    return 0;
}
