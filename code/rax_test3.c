// 演示函数3: 演示部分片段代码
#include <stdlib.h>
#include "../redis-5.0.0/src/rax.c"
#include "../redis-5.0.0/src/zmalloc.c"

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

int get_node_key(raxIterator *it) {
    // raxSeek()函数中标记为5b的代码段，笔者将其封装成一个函数
    if (!raxStackPush(&it->stack,it->node)) return 0;
    for (size_t j = 1; j < it->stack.items; j++) {
        raxNode *parent = it->stack.stack[j-1];
        raxNode *child = it->stack.stack[j];
        if (parent->iscompr) {
            if (!raxIteratorAddChars(it,parent->data,parent->size))
               return 0;
        } else {
            raxNode **cp = raxNodeFirstChildPtr(parent);
            unsigned char *p = parent->data;
            while(1) {
               raxNode *aux;
               memcpy(&aux,cp,sizeof(aux));
               if (aux == child) break;
               cp++;
               p++;
            }
            if (!raxIteratorAddChars(it,p,1)) return 0; 
        }
    }
    raxStackPop(&it->stack);
    return 1;
}

int main() {
    rax *r = gen_rax();
    // 匹配键helyz
    raxIterator rt;
    raxStart(&rt, r);
    raxIterator *it = &rt;

    int splitpos = 0;
    size_t i = raxLowWalk(it->rt, "helyz", 5, &it->node, NULL, &splitpos, &it->stack);
    // i = 3  splitpos = 0   "lo"
    printf("i=%d, j=%d, h->node->data=%s\n", i, splitpos, it->node->data);
    
    get_node_key(it); 

    // "hel"  3
    printf("match node represent key=%s, len=%d\n", it->key, it->key_len);

    return 0;
}
