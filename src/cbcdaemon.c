/**
 * cbc commands
 * @file cbcdaemon.c
 * @author Pan, Shi Zhu
 */

#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <locale.h>
#include "luatools.h"

/*
 * sample dummy function
 */
static int l_hello(lua_State *L)
{
    printf("Hello, world\n");
    return 0;
}

static const luaL_Reg cbclib[] = {
    { "hello", l_hello },
};

static int luaopen_cbc(lua_State *L)
{
    /* register functions as global */
    l_register_global(L, cbclib);

    return 0;
}

/*
 * on success, return 0
 * return non-zero on error, the error must be caught
 */
lua_State *cbc_init()
{
    /* init the state machine */
    lua_State *L;
    setlocale(LC_ALL, "");
    L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_tools(L);
    luaopen_cbc(L);

    return L;
}

int cbc_run(lua_State *L)
{
    int result = luaL_loadfile(L, "server.lua") ;
    if (result != 0) {
        printf("pcall error=%d, %s", result, lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        /* my initializations */

        result = lua_pcall(L, 0, 0, 0);
        if (result != 0) {
            printf("pcall error=%d, %s", result, lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    lua_State *L = cbc_init();

    cbc_run(L);

    return 0;
}

