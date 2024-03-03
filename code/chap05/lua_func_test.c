#include <stdio.h> 
#include <string.h>
#include "lua.h" 
#include "lauxlib.h"
#include "lualib.h"
#include "../../redis-learning/redis-code/redis-5.0.0/src/sds.c"
#include "../../redis-learning/redis-code/redis-5.0.0/src/zmalloc.c"


int luaRedisGenericCommand(lua_State *lua, int raise_error) {
    printf("调用C中的函数...\n");
    int j, argc = lua_gettop(lua);  // 获取调用该函数时的参数个数，即栈中元素个数
    static char **argv = NULL;
    static int argv_size = 0;

    if (argv_size < argc) {
        argv = zrealloc(argv,sizeof(char*)*argc);
        argv_size = argc;
    }

    sds cmdstr = sdsempty();
    for (j = 0; j < argc; j++) {   // 由底向上遍历栈，获取传入参数
        char *obj_s;
        size_t obj_len;
        char dbuf[64];
        if (lua_type(lua,j+1) == LUA_TNUMBER) {       // 数值类型
            lua_Number num = lua_tonumber(lua,j+1);   // 得到相应类型的结果

            obj_len = snprintf(dbuf,sizeof(dbuf),"%.17g",(double)num); // 转成字符串
            obj_s = dbuf;
        } else {
            obj_s = (char*)lua_tolstring(lua,j+1,&obj_len);  
            if (obj_s == NULL) break;  /* 不是string类型 */
        }
        argv[j] = obj_s;
        sdscat(cmdstr, obj_s);
        sdscat(cmdstr, " ");
    }

    printf("调用redis.call()函数执行的命令为:%s\n", cmdstr);
    return 1;
}

int luaRedisCallCommand(lua_State *lua) {
    return luaRedisGenericCommand(lua,1);
}

int main() {
    char buff [256];
    int error;

    lua_State *lua = lua_open();
    lua_newtable(lua);           // 创建一个表，位于栈顶

    lua_pushstring(lua,"call");  
    lua_pushcfunction(lua,luaRedisCallCommand);
    lua_settable(lua,-3);       // 设置表，该表在栈中的索引为-3

    lua_setglobal(lua,"redis"); 
    if(fgets(buff, sizeof(buff), stdin) != NULL) {   // 读取一行内容
        error = luaL_loadstring(lua, buff) || lua_pcall(lua, 0, 0, 0);
        if(error) {
            fprintf(stderr, "%s\n", lua_tostring(lua, -1)); 
            lua_pop(lua, 1);  // 从栈中弹出错误信息
        }
    }

    lua_close(lua);
    return 0;
}
