#include <stdio.h> 
#include <string.h>
#include "lua.h" 
#include "lauxlib.h"
#include "lualib.h"


int main (void) { 
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
        printf("> ");
}
    lua_close(lua);
}
