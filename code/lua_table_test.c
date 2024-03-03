#include <stdio.h> 
#include <string.h>
#include "../../lua-5.1/src/lua.h" 
#include "../../lua-5.1/src/lauxlib.h"
#include "../../lua-5.1/src/lualib.h"


void get_lua_var(lua_State *lua, char *name) {
    const char *p;
    char buf[64];
    int b;
    lua_getglobal(lua, name);   // 将lua中的全局变量name推入栈中
    int t = lua_type(lua, -1);  // 获取栈顶元素类型
    switch (t)
    {
    case LUA_TSTRING:
        p = lua_tostring(lua, -1);
        printf("Lua中变量%s的值为%s (string)", name, p);
        lua_getglobal(lua, "alias");  
        if(lua_type(lua, -1) != LUA_TNIL && lua_istable(lua, -1)) {
            lua_getfield(lua, -1, p);
            if(lua_type(lua, -1) == LUA_TSTRING) {
                printf(", %s的别名为%s", name, lua_tostring(lua, -1));
            }
        }
        printf("\n");
        break;
    case LUA_TNUMBER:
        printf("Lua中变量%s的值为%d (integer)\n", name, (int)lua_tonumber(lua, -1));
        break;
    case LUA_TBOOLEAN:
        b = lua_toboolean(lua, -1);
        sprintf(buf, "%s_table", name);
        lua_getglobal(lua, buf);
        if(lua_type(lua, -1) != LUA_TNIL && lua_istable(lua, -1)) {
            b++;
            lua_pushinteger(lua, b);  
            lua_gettable(lua, -2); // 表在栈中索引为-2的位置, 栈顶为键
            printf("Lua中变量%s的值为%s (boolean -> string)\n", name, lua_tostring(lua, -1));
        }
        break;
    default:
        break;
    }
    lua_settop(lua, 0);   // 清空栈
}

int main() {
    char buff [256];
    int error;
    lua_State *lua = lua_open();  // 打开lua
    luaL_openlibs(lua);              // 打开标准库
    printf("> ");
    while(fgets(buff, sizeof(buff), stdin) != NULL) {
        buff[strlen(buff) - 1] = '\0';   // 去掉最后的换行符
        if(!strcasecmp(buff, "quit") || !strcasecmp(buff, "exit")) {
            break;
        }
        error = luaL_loadstring(lua, buff) || lua_pcall(lua, 0, 0, 0);
        if(error) {
            fprintf(stderr, "%s\n", lua_tostring(lua, -1)); 
            lua_pop(lua, 1);  // 从栈中弹出错误信息
        }
        // 监控变量name、age和sex
        get_lua_var(lua, "name");
        get_lua_var(lua, "age");
        get_lua_var(lua, "sex");

        printf("> ");
    }

    lua_close(lua);
    return 0;
}
