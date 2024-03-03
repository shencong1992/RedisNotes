#include <stdio.h> 
#include <string.h>
#include "../../lua-5.1/src/lua.h" 
#include "../../lua-5.1/src/lauxlib.h"
#include "../../lua-5.1/src/lualib.h"

static stackDump(lua_State *lua) {
    int i;
    int top = lua_gettop(lua);     // 获取栈顶索引，也即栈中元素个数
    printf("------stack-----\n");
    for(i = top; i >= 1; i--) {
        char *istop = i == top ? "top" : "";
        int t = lua_type(lua, i);  // 获取索引i位置的元素类型
        int n_t = -(top) + i - 1;  // 反向索引值
        switch (t)
        {
        case LUA_TSTRING:
            printf("%d:%s:%d %s\n", i, lua_tostring(lua, i), n_t, istop);
            break;
        case LUA_TNUMBER:
            printf("%d:%g:%d %s\n", i, lua_tonumber(lua, i), n_t, istop);
            break;
        case LUA_TBOOLEAN:
            printf("%d:%s:%d %s\n", i, lua_toboolean(lua, i) ? "true" : "false", n_t, istop);
            break;
        default:
            printf("%d:%s:%d %s\n", i, lua_typename(lua, t), n_t, istop);
            break;
        }
    }
    printf("----------------\n\n");
}

int main() {
    lua_State *lua = lua_open();
    luaL_openlibs(lua);

    // 向栈中推入数据  
    lua_pushstring(lua, "hello");
    lua_pushnil(lua);
    lua_pushboolean(lua, 0);
    lua_pushnumber(lua, 10);

    stackDump(lua);   // 打印栈中数据

    lua_pop(lua, 1); // 弹出1个元素
    stackDump(lua);

    lua_pop(lua, 2); // 弹出2个元素
    stackDump(lua);

    lua_close(lua);
    return 0;
}
