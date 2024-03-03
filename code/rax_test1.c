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

    // 获取键"footer"对应的值
    printf("footer=%s\n", raxFind(r, "footer", 6));

    // raxTryInsert()函数, 如果存在, 则不会覆盖
    raxTryInsert(r, "footer", 6, "mn", NULL);

    printf("after try insert, footer=%s\n", raxFind(r, "footer", 6));

    raxInsert(r, "footer", 6, "mn", NULL);
    printf("after insert, footer=%s\n", raxFind(r, "footer", 6));

    // 演示raxRemove()函数
    raxRemove(r, "footer", 6, NULL);
    printf("after remove footer...\n");
    raxShow(r);

    raxFree(r);
    return 0;
}
