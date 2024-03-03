#include <stdio.h> 
#include <string.h>
#include "lua.h" 
#include "lauxlib.h"
#include "lualib.h"
#include "../../redis-learning/redis-code/redis-5.0.0/src/sds.c"
#include "../../redis-learning/redis-code/redis-5.0.0/src/zmalloc.c"

void luaPushError(lua_State *lua, char *error) {
    lua_newtable(lua);
    lua_pushstring(lua, "err");
    lua_pushstring(lua, error);
    lua_settable(lua,-3);   // 设置t["err"] = error，之后该表t位于栈顶
}

int luaRaiseError(lua_State *lua) {
    lua_pushstring(lua,"err");
    lua_gettable(lua,-2);   // 获取t["err"]并保存到栈顶
    return lua_error(lua);  // 调用lua_error()函数打印栈顶错误信息
}

int luaRedisGenericCommand(lua_State *lua, int raise_error) {
    int j, argc = lua_gettop(lua);  // 获取调用该函数时的参数个数
    static double *argv = NULL;
    static int argv_size = 0;

    if (argv_size < argc) {
        argv = zrealloc(argv,sizeof(char*)*argc);
        argv_size = argc;
    }

    sds cmdstr = sdsempty();
    for (j = 0; j < argc; j++) {
        char *obj_s;
        size_t obj_len;
        char dbuf[64];
        if(lua_type(lua,j+1) == LUA_TNUMBER) {
            lua_Number num = lua_tonumber(lua,j+1);
            argv[j] = (double)num;
        } else {
            luaPushError(lua,
            "Lua redis.add() arguments must be numbers");   // 推入指定的错误信息
            return raise_error ? luaRaiseError(lua) : 1;
        }
    }

    double sum = 0;
    for (j = 0; j < argc; j++) {
        sum += argv[j];
    }
    printf("计算结果为:%f\n", sum);
}

int luaRedisAddCommand(lua_State *lua) {
    return luaRedisGenericCommand(lua,1);
}

int luaRedisPAddCommand(lua_State *lua) {
    return luaRedisGenericCommand(lua,0);
}

int main() {
    char buff [256];
    int error;

    lua_State *lua = lua_open();
    lua_newtable(lua);           // 创建一个表，位于栈顶

    lua_pushstring(lua,"add");  
    lua_pushcfunction(lua,luaRedisAddCommand);
    lua_settable(lua,-3);       // 设置表，该表在栈中的索引为-3

    lua_pushstring(lua,"padd");  
    lua_pushcfunction(lua,luaRedisPAddCommand);
    lua_settable(lua,-3);       // 设置表，该表在栈中的索引为-3

    lua_setglobal(lua,"redis"); 
    printf("> ");
    while(fgets(buff, sizeof(buff), stdin) != NULL) {   // 循环读取一行内容
        buff[strlen(buff) - 1] = '\0';                       // 去掉最后的换行符
        if(!strcasecmp(buff, "quit") || !strcasecmp(buff, "exit")) {
            break;
        }
        error = luaL_loadstring(lua, buff) || lua_pcall(lua, 0, 0, 0);
        if(error) {
            fprintf(stderr, "%s\n", lua_tostring(lua, -1)); 
            lua_pop(lua, 1);  // 从栈中弹出错误信息
        }
        printf("> ");
    }

    lua_close(lua);
    return 0;
}
