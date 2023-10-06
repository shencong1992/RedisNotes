// 源文件: rax_padding_test.c 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "../redis-5.0.0/src/zmalloc.c"
#include "../redis-5.0.0/src/listpack.c"
#include "../redis-5.0.0/src/rax.c"

#define raxPadding(nodesize) ((sizeof(void*)-((nodesize+4) % sizeof(void*))) & (sizeof(void*)-1))

int main() {
    char s[64];
    int64_t v;
    printf("sizeof(raxNode) = %d, sizeof(raxNode*) = %d\n", sizeof(raxNode), sizeof(raxNode*));
    while(1) {
        printf("input a node size: ");
        scanf("%s", s);
        if (strcmp(s, "exit") == 0) break;
        // 位于listpack.c中, 用于将字符串转换成64位整数, 5.1.2节中有介绍到
        lpStringToInt64(s, strlen(s), &v);  
        printf("%d\n", v + raxPadding(v));
    }
    return 0;
}
