// 演示函数2: 测试rax.c中的raxLowWalk()函数
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

int main() {
    rax *r = gen_rax();

    // 匹配键footxkm
    size_t i;
    int j = 0;
    raxNode *h, **parentlink;
    i = raxLowWalk(r, "footxkm", 6, &h, &parentlink, &j, NULL);
    printf("i=%d, j=%d, h->node->data=%s\n", i, j, h->data);
    printf("node addr=0x%x\n", *parentlink);
    printf("match node addr=%p\n", h);

    // 根据parentlink可以找到其兄弟节点
    raxNode **n = parentlink - 1;
    printf("last child node: data=%s, size=%d\n", (*n)->data, (*n)->size);
 
    raxNode *p = (raxNode *)(((char *)n) - 4 - 4);
    printf("parent node data=%s, node size=%d\n", p->data, p->size);
    return 0;
}
