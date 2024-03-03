#include <stdlib.h>
#include "../redis-5.0.0/src/rax.c"
#include "../redis-5.0.0/src/zmalloc.c"

rax *gen_rax() {
    // 和实战1中的不同，笔者为了测试效果，多加了几对key-value数据
    rax *r = raxNew();
    raxInsert(r, "hello", 5, "world", NULL);
    raxInsert(r, "foote", 5, "012", NULL);
    raxInsert(r, "footer", 6, "123", NULL);
    raxInsert(r, "footx", 5, "mn1", NULL);
    raxInsert(r, "footxmy", 7, "abc", NULL);
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

void print_iterator(raxIterator *it, char *tips) {
    char key[64];
    for(int i = 0; i < it->key_len; i++) {  // 打印迭代器所在raxNode节点代表的路径字符串
        key[i] = it->key[i];
    }
    key[it->key_len] = '\0';
    printf("%s: data=%s, size=%d, key=%s\n", tips, it->node->data, it->node->size, key);    
}

size_t test_start(raxIterator *it, char *ele, int len, int *splitpos) {
    // 测试开始, 需要做的工作就是取消EOF标志、调用raxLowWalk()函数， 打印终止节点信息
    ele[len] = '\0';

    printf("\n------------");
    printf("compare string = %s---------------------\n", ele);

    // 参见raxSeek()函数，第一步就是要取消EOF标志，否则在raxIteratorPrevStep()函数中将直接返回
    it->flags &= ~RAX_ITER_EOF;
    size_t i = raxLowWalk(it->rt, ele, len, &it->node, NULL, splitpos, &it->stack);
    get_node_key(it);
    print_iterator(it, "after raxLowWalk() match");
    printf("if条件: 是否完全匹配? %d, 是否终止在压缩节点? %d\n",  i == len, it->node->iscompr );
    return i;
}

int test_end(raxIterator *it, char *ele, int len) {
    // 测试结束, 就是调用raxSeek()查看小于的结果, 看同笔者在代码中的操作结果是否一致
    raxSeek(it, "<", ele, len);
    print_iterator(it, "raxSeek() find raxNode");
    raxStop(it);
}

int main(){
    int splitpos = 0;
    int len;
    size_t i;

    rax *r = gen_rax();    
    raxIterator rt;
    raxIterator *it;

    // 第1种情况
    char s1[10] = "footabc";
    len = strlen(s1);
    raxStart(&rt, r);
    it = &rt;
    
    i = test_start(it, s1, len, &splitpos);
    if (!raxIteratorAddChars(it,s1+i,1)) return 0;
    it->flags &= ~RAX_ITER_JUST_SEEKED;
    if (!raxIteratorPrevStep(it, 1)) return 0;
    it->flags |= RAX_ITER_JUST_SEEKED;
    print_iterator(it, "after raxIteratorPrevStep(), noup=1"); 
    test_end(it, s1, len);

    // 第2种情况
    char s2[10] = "footxma";
    len = strlen(s2);
    raxStart(&rt, r);
    it = &rt;

    i = test_start(it, s2, len, &splitpos);
    int nodechar = it->node->data[splitpos];
    int keychar = s2[i];
    it->flags &= ~RAX_ITER_JUST_SEEKED;
    printf("走到else分支\n");
    if (!raxIteratorAddChars(it,it->node->data,it->node->size))
        return 0;
    if (!raxIteratorPrevStep(it,1)) return 0;
  
    it->flags |= RAX_ITER_JUST_SEEKED;
    print_iterator(it, "after raxIteratorPrevStep()"); 
    test_end(it, s2, len);

    // 第3种情况
    char s3[10] = "footxm";
    len = strlen(s3);
    raxStart(&rt, r);
    it = &rt;

    test_start(it, s3, len, &splitpos);
    if (it->node->iscompr && it->node->iskey && splitpos) {
        printf("满足查找的上一个节点条件...\n");
    }
    print_iterator(it, "after raxIteratorPrevStep(), noup=0"); 
    test_end(it, s3, len);

    // 第4种情况
    char s4[10] = "hel";
    len = strlen(s4);
    raxStart(&rt, r);
    it = &rt;

    test_start(it, s4, len, &splitpos);
    it->flags &= ~RAX_ITER_JUST_SEEKED;
    if (!raxIteratorPrevStep(it,0)) return 0;
    it->flags |= RAX_ITER_JUST_SEEKED;
    print_iterator(it, "after raxIteratorPrevStep(), noup=0");
    test_end(it, s4, len);

    return 0;
}
