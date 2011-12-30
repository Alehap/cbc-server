/**
 * @file ltools.c
 * @brief my personal tools for Lua
 *
 *  Copyright (C) 2012  Pan, Shi Zhu
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* for lua_ and luaL_ */
#include <lua.h>
#include <lauxlib.h>
/* for printf */
#include <stdio.h>
/* for strerror */
#include <string.h>
/* for errno */
#include <errno.h>
/* for syslog */
#include <syslog.h>
/* for isprint() */
#include <ctype.h>
#include <stdlib.h>
#include <wctype.h>
/* for std types */
#include <stdbool.h>
#include <stdint.h>

#define LTOOLS_C
#include "luatools.h"

/* define this to 0 register in tools.*, otherwise register in global */
#define REGISTER_GLOBAL 0

#ifdef ONLY_FOR_DEMO
/**
 * @return: integer
 */
static int l_hello(lua_State * L)
{
    printf("Hello!\n");
    lua_pushinteger(L, 0);
    return 1;
}
#endif

/**
 * Not to be called in Lua, it should be called within C functions.
 */
void l_stackdump(lua_State *L, const char *func, int line)
{
    int top = lua_gettop(L);
    printf("%s %d: ", func, line);  /* end the listing */
    for (int i = 1; i <= top; i++) {  /* repeat for each level */
        int type = lua_type(L, i);
        switch (type) {

        case LUA_TSTRING:  /* strings */
            printf("'%s'", lua_tostring(L, i));
            break;

        case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;

        case LUA_TNUMBER:  /* numbers */
            printf("%s", lua_tostring(L, i));
            break;

        default:  /* other values */
            printf("%s", lua_typename(L, type));
            break;

        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

#ifdef ONLY_FOR_DEMO
/**
 * @param: value to be print
 */
static int l_print_table(lua_State * L)
{
    if (!lua_istable(L, 1)) {
        /* not a table, calling print */
        lua_getglobal(L, "print");
        lua_insert(L, -2);
        lua_call(L, 1, 0);
    } else {
        /* is a table in the stack at index 1 */
        for (lua_pushnil(L); lua_next(L, 1) != 0; ) {
            /* save the key for the next lua_next */
            lua_pushvalue(L, -2);
            lua_insert(L, -2);
            /* call the print function */
            lua_getglobal(L, "print");
            lua_insert(L, -3);
            lua_call(L, 2, 0);	/* call print with key and value */
        }

    }
    return 0;
}
#endif

/**
 * @param: string
 * @return: hex dump
 */
static int l_hex_dump(lua_State * L)
{
    const char *str;
    size_t len;
    str = luaL_checklstring(L, 1, &len);
    /* guard empty string */
    if (len == 0) {
        lua_pushliteral(L, "");
        return 1;
    }

    char buf[len*3];	/* C99 VLA */
    for (size_t i=0; i<len; i++ ) {
        sprintf(&buf[3*i], "%02x", (unsigned char)str[i]);
        buf[3*i+2] = ' ';
    }
    buf[sizeof buf -1] = '\0';
    lua_pushlstring(L, buf, len*3);
    return 1;
}

/**
 * @param: the log msg
 * @param: (optional) the log level, default LOG_INFO
 */
static int l_syslog(lua_State * L)
{
    const char *log = lua_tostring(L, 1);
    int log_level = luaL_optint(L, 2, LOG_INFO);
    syslog(log_level, "%s", log);
    return 0;
}

/**
 * @param: num1
 * @param: num2
 * @return: result
 */
static int l_bitand(lua_State * L)
{
    unsigned int num1 = luaL_checkinteger(L, 1);
    unsigned int num2 = luaL_checkinteger(L, 2);
    lua_pushinteger(L, num1 & num2);
    return 1;
}

/**
 * @param: num1
 * @param: num2
 * @return: result
 */
static int l_bitor(lua_State * L)
{
    unsigned int num1 = luaL_checkinteger(L, 1);
    unsigned int num2 = luaL_checkinteger(L, 2);
    lua_pushinteger(L, num1 | num2);
    return 1;
}

/**
 * @param: num1
 * @param: num2
 * @return: result
 */
static int l_bitxor(lua_State * L)
{
    unsigned int num1 = luaL_checkinteger(L, 1);
    unsigned int num2 = luaL_checkinteger(L, 2);
    lua_pushinteger(L, num1 ^ num2);
    return 1;
}

/**
 * @param: num1
 * @return: result
 */
static int l_bitnot(lua_State * L)
{
    unsigned int num1 = luaL_checkinteger(L, 1);
    lua_pushinteger(L, ~num1);
    return 1;
}

/* only for demo and it is actually not used */
#ifdef ONLY_FOR_DEMO
/**
 * @param: the string (upvalue1)
 * @param: the index (upvalue2)
 * @return: result
 */
static int c_striter(lua_State * L)
{
    size_t size;
    const char *str = lua_tolstring(L, lua_upvalueindex(1), &size);
    ptrdiff_t index = lua_tointeger(L, lua_upvalueindex(2));

    if (index < size) {
        /* update the upvalue */
        lua_pushinteger(L, index+1);
        lua_replace(L, lua_upvalueindex(2));
        /* push the result */
        lua_pushinteger(L, (unsigned int)str[index]);
    } else {
        /* push the result */
        lua_pushnil(L);
    }
    return 1;
}
/**
 * @param: the string
 * @return: the closure
 */
static int l_striter(lua_State * L)
{
    size_t size;
    const char *str = luaL_checklstring(L, 1, &size);
    lua_pushlstring(L, str, size);
    lua_pushinteger(L, 0);
    lua_pushcclosure(L, c_striter, 2);
    return 1;
}
#endif

/**
 * @return: the uptime
 */
static int l_uptime(lua_State * L)
{
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        return luaL_error(L, "l_uptime error");
    }
    unsigned long sec, msec;
    int ret = fscanf(fp, "%lu.%2lu", &sec, &msec);
    if (ret == EOF) {
        return luaL_error(L, "/proc/uptime has an unexpected format");
    }

    lua_pushinteger(L, (sec & 0xffffff)*100+msec);
    fclose(fp);
    return 1;
}

/**
 * @param: the string (at stack -1)
 * @return: the quoted string (push to stack -1)
 */
static int l_repr_string(lua_State *L)
{
    size_t size;
    const char *src = lua_tolstring(L, -1, &size);
    /* calculate the single-quote vs. double-quote */
    size_t single_quote=0, double_quote=0;
    for (size_t i=0; i<size; i++) {
        if (src[i] == '"')
            ++double_quote;
        if (src[i] == '\'')
            ++single_quote;
    }
    char quote = (double_quote > single_quote) ? '\'' : '"';
    wchar_t wstr[size+1];
    size_t retsize = mbstowcs(wstr, src, size+1);

    /* start of luaL_Buffer */
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaL_addchar(&b, quote);

    /* if not a valid utf-8 string we use isprint() to check printable char */
    if (retsize == (size_t)-1) {
        for (; size--; src++) {
            switch (*src) {
            case '\\':
                luaL_addlstring(&b, "\\\\", 2);
                break;
            case '\r':
                luaL_addlstring(&b, "\\r", 2);
                break;
            case '\n':
                luaL_addlstring(&b, "\\n", 2);
                break;
            default:
                if (*src == quote) {
                    luaL_addchar(&b, '\\');
                    luaL_addchar(&b, quote);
                } else if (isprint(*src)) {
                    luaL_addchar(&b, *src);
                } else {
                    char repr_char[5];
                    /* forget to cast unsigned leads to segmentation fault */
                    sprintf(repr_char, "\\%03u", (unsigned char)(*src));
                    luaL_addlstring(&b, repr_char, 4);
                }
                break;
            }
        }
    } else {
        /* for utf-8 string we use iswprint() */
        for (size_t i = 0; i < retsize; i++) {
            switch (wstr[i]) {
            case L'\\':
                luaL_addlstring(&b, "\\\\", 2);
                break;
            case L'\r':
                luaL_addlstring(&b, "\\r", 2);
                break;
            case L'\n':
                luaL_addlstring(&b, "\\n", 2);
                break;
            default:
                if (wstr[i] == (wchar_t)quote) {
                    luaL_addchar(&b, '\\');
                    luaL_addchar(&b, quote);
                } else {
                    char utf8str[MB_CUR_MAX];
                    int wssize = wctomb(utf8str, wstr[i]);
                    /* most utf-8 character should be printable */
                    if (iswprint(wstr[i])) {
                        luaL_addlstring(&b, utf8str, wssize);
                    } else {
                        char repr_char[wssize*4+1];
                        for (int j=0; j<wssize; j++) {
                            /* forget to cast unsigned leads to segmentation fault */
                            sprintf(repr_char, "\\%03u", (unsigned char)utf8str[j]);
                            luaL_addlstring(&b, repr_char, 4);
                        }
                    }
                }
                break;
            }
        }
        size_t zs = strlen(src);
        if (zs != size) {
            for (; zs<size; zs++) {
                switch (src[zs]) {
                case '\\':
                    luaL_addlstring(&b, "\\\\", 2);
                    break;
                case '\r':
                    luaL_addlstring(&b, "\\r", 2);
                    break;
                case '\n':
                    luaL_addlstring(&b, "\\n", 2);
                    break;
                default:
                    if (src[zs] == quote) {
                        luaL_addchar(&b, '\\');
                        luaL_addchar(&b, quote);
                    } else if (isprint(src[zs])) {
                        luaL_addchar(&b, src[zs]);
                    } else {
                        char repr_char[5];
                        /* forget to cast unsigned leads to segmentation fault */
                        sprintf(repr_char, "\\%03u", (unsigned char)(src[zs]));
                        luaL_addlstring(&b, repr_char, 4);
                    }
                    break;
                }
            }
        }
    }
    luaL_addchar(&b, quote);
    luaL_pushresult(&b);
    /* end of luaL_Buffer */

    return 1;
}

/**
 * @param: the string (at stack -1)
 * @return: boolean value (stack balanced)
 */
static bool l_checkid(lua_State *L)
{
    size_t size;
    const char *keystr = lua_tolstring(L, -1, &size);

    /* check if it is a valid identifier */
    if (!isalpha(keystr[0]) && !(keystr[0]=='_'))
        return false;
    for (size_t i = 1; i < size; ++i) {
        if (!isalnum(keystr[i]) && !(keystr[i]=='_'))
            return false;
    }
    /* check if it is lua keywords */
    lua_getfield(L, LUA_REGISTRYINDEX, "tools.l_keywords");
    lua_getfield(L, -1, keystr);
    bool iskeyword = lua_toboolean(L, -1);
    lua_pop(L, 2);
    return !iskeyword;
}

static int l_compact_repr_slice(lua_State *L);
/**
 * @param: the key (at stack 1)
 * @param: the cycle table (at stack 2)
 * @return: the key string
 */
static int l_repr_key(lua_State *L)
{
    if (lua_type(L, 1) == LUA_TSTRING) {
        lua_pushvalue(L, 1);	/* the key */
        bool isidentifier = l_checkid(L);

        l_repr_string(L);	/* generate repr string on top */
        size_t size;
        const char *reprstr = lua_tolstring(L, -1, &size);
        if (isidentifier) {
            lua_pop(L, 1);	/* just leave the backuped key on top */
        } else {
            lua_pushfstring(L, "[%s]", reprstr);
        }
    } else {
        lua_pushcfunction(L, l_compact_repr_slice);
        lua_insert(L, -3);
        lua_call(L, 2, 1);
        size_t size;
        const char *reprstr = lua_tolstring(L, -1, &size);
        lua_pushfstring(L, "[%s]", reprstr);
    }
    return 1;
}

/**
 * @param: the table (at stack 1)
 * @param: the cycle table (at stack 2)
 * @return: the table string
 */
static int l_compact_repr_table(lua_State *L)
{
    lua_pushvalue(L, 1);	/* duplicate the table in stack 3 */
    lua_rawget(L, 2);		/* get from the cycle table 2 */
    if (lua_isnil(L, -1)) {
        			/* leave the nil at stack 3 */
        lua_pushfstring(L, "te(%d)", 
                ((size_t)lua_topointer(L, 1))&(8192-1));	/* set the value in stack 4 */
        lua_pushvalue(L, 1);	/* push in stack 5 */
        lua_insert(L, -2);	/* swap 4/5 to set the key */
        lua_rawset(L, 2);	/* adjust the cycle table and pop 4,5 */
        lua_pushnil(L);		/* make a room at stack 4 */

        /* luaL_Buffer START */
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        luaL_addlstring(&b, "{ ", 2);

        /* iterate the table */
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pushvalue(L, 4)) {
            lua_replace(L, 3);	/* backups the value in stack 3 */
            lua_replace(L, 4);	/* backups the key in stack 4 */
            /* repr the key */
            lua_pushcfunction(L, l_repr_key);
            lua_pushvalue(L, 4);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            luaL_addvalue(&b);	/* add the return value into buffer */
            luaL_addlstring(&b, " = ", 3);
            /* repr the value */
            lua_pushcfunction(L, l_compact_repr_slice);
            lua_pushvalue(L, 3);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            luaL_addvalue(&b);	/* add the return value into buffer */
            luaL_addlstring(&b, ", ", 2);
        }

        luaL_addchar(&b, '}');
        luaL_pushresult(&b);
        /* luaL_Buffer END */
    } else {
        /* just use the return value of gettable as the result */
    }

    return 1;
}

static int l_pretty_repr_slice(lua_State *L);
/**
 * @param: the table (at stack 1)
 * @param: the cycle table (at stack 2)
 * @return: the table string
 */
static int l_pretty_repr_table(lua_State *L)
{
    lua_pushvalue(L, 1);	/* duplicate the table in stack 3 */
    lua_rawget(L, 2);		/* get from the cycle table 2 */
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        /* save the table item into cycle table */
        lua_pushfstring(L, "table_(%p)", lua_topointer(L, 1)); /* put in stack 3 */
        lua_pushvalue(L, 1);	/* push key in stack 4 */
        lua_pushvalue(L, 3);	/* push value in stack 5 */
        lua_rawset(L, 2);	/* adjust the cycle table and pop 4,5 */
        /* save the level into cycle table */
        lua_pushliteral(L, "level");
        lua_rawget(L, 2);	/* get the level into stack 4 */
        int level = 1;
        if (!lua_isnil(L, -1)) {
            level += lua_tointeger(L, -1);
        }
        lua_pushliteral(L, "level");
        lua_pushinteger(L, level);
        lua_rawset(L, 2);	/* save the level and pop 5,6 */

        /* luaL_Buffer START */
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        if (1) {
            luaL_addlstring(&b, "{\n", 2);
        } else {		/* optional: suffix a pointer behind */
            lua_pushfstring(L, "{ -- %p\n", lua_topointer(L, 1));
            luaL_addvalue(&b);	/* add the pointer value into buffer */
        }

        /* iterate the table */
        for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pushvalue(L, 4)) {
            lua_replace(L, 3);	/* backups the value in stack 3 */
            lua_replace(L, 4);	/* backups the key in stack 4 */
            for (int i=level; i--; )
                luaL_addchar(&b, '\t');
            /* repr the key */
            lua_pushcfunction(L, l_repr_key);
            lua_pushvalue(L, 4);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            luaL_addvalue(&b);	/* add the return value into buffer */
            luaL_addlstring(&b, " = ", 3);
            /* repr the value */
            lua_pushcfunction(L, l_pretty_repr_slice);
            lua_pushvalue(L, 3);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            luaL_addvalue(&b);	/* add the return value into buffer */
            luaL_addlstring(&b, ",\n", 2);
        }

        --level;		/* decrease level when table finished */
        for (int i=level; i--; )
            luaL_addchar(&b, '\t');
        luaL_addchar(&b, '}');
        luaL_pushresult(&b);
        /* luaL_Buffer END */
        lua_pushliteral(L, "level");
        lua_pushinteger(L, level);
        lua_rawset(L, 2);	/* save the level and pop */
    } else {
        /* just use the return value of gettable as the result */
    }

    return 1;
}

/**
 * @param: the value (at stack -2)
 * @param: the cycle table (at stack -1)
 * @return: the string
 */
static int l_compact_repr_slice(lua_State *L)
{
    int type = lua_type(L, -2);
    switch (type) {
    case LUA_TNIL: 
        lua_pushliteral(L, "nil");
        break;
    case LUA_TNUMBER: 
        lua_pushvalue(L, -2);	/* copy the number */
        lua_pushstring(L, lua_tostring(L, -1));
        break;
    case LUA_TBOOLEAN: 
        lua_pushstring(L, (lua_toboolean(L, -2) ? "true" : "false"));
        break;
    case LUA_TSTRING: 
        lua_pop(L, 1);
        l_repr_string(L);
        break;
    case LUA_TTABLE: 
        lua_pushcfunction(L, l_compact_repr_table);
        lua_pushvalue(L, -3);	/* the table and cycle table are copied */
        lua_pushvalue(L, -3);
        lua_call(L, 2, 1);
        break;
    case LUA_TFUNCTION: 
    case LUA_TUSERDATA: 
    case LUA_TTHREAD: 
    case LUA_TLIGHTUSERDATA: 
        lua_pop(L, 1);
        const char *prefix;
        prefix= lua_typename(L, type);
        const void *ptr;
        ptr = lua_topointer(L, -1);
        lua_pushfstring(L, "%c%c(%d)", prefix[0], prefix[strlen(prefix)-1], 
                ((size_t)ptr)&(8192-1));
        break;
    default:
        lua_pushfstring(L, "unknown_(%d)", type);
        break;
    }
    return 1;
}

/**
 * @param: the value (at stack -2)
 * @param: the cycle table (at stack -1)
 * @return: the string
 */
static int l_pretty_repr_slice(lua_State *L)
{
    int type = lua_type(L, -2);
    switch (type) {
    case LUA_TSTRING: 
        lua_pop(L, 1);
        l_repr_string(L);
        break;
    case LUA_TTABLE: 
        lua_pushcfunction(L, l_pretty_repr_table);
        lua_pushvalue(L, -3);	/* the table and cycle table are copied */
        lua_pushvalue(L, -3);
        lua_call(L, 2, 1);
        break;
    case LUA_TNIL: 
    case LUA_TNUMBER: 
    case LUA_TBOOLEAN: 
        lua_pop(L, 1);
        lua_getglobal(L, "tostring");
        lua_insert(L, -2);
        lua_call(L, 1, 1);
        break;
    case LUA_TFUNCTION: 
    case LUA_TUSERDATA: 
    case LUA_TTHREAD: 
    case LUA_TLIGHTUSERDATA: 
        lua_pop(L, 1);
        const void *p;
        p = lua_topointer(L, -1);
        lua_pushfstring(L, "%s_(%p)", lua_typename(L, type), p);
        break;
    default:
        lua_pushfstring(L, "unknown_(%d)", type);
        break;
    }
    return 1;
}

/**
 * @param: the values (0, 1 or more)
 * @return: the string
 */
static int l_temp_repr(lua_State *L, lua_CFunction slice_fn)
{
    if (lua_isnone(L, 1)) {
        /* no argument, returns empty string */
        lua_pushliteral(L, "");
    } else if (lua_isnone(L, 2)) {
        /* single argument, no tuple, directly call repr_slice() with an empty
         * table */
        lua_newtable(L);
        slice_fn(L);
    } else {
        /* multiple argument */

        /* argc must be saved because luaL_Buffer will use stack */
        int argc = lua_gettop(L)+1;
        lua_newtable(L);	/* save a new table at stack argc */

        /* buffer starts here */
        luaL_Buffer b;
        luaL_buffinit(L, &b);
        for (int i=1; i<argc; i++) {
            /* from the next element of tuple, add ", " */
            if (i != 1) {
                luaL_addstring(&b, ", ");
            }
            /* call repr_slice with standard lua call interface
             * so that the stack are balanced for luaL_Buffer */
            lua_pushcfunction(L, slice_fn);
            lua_pushvalue(L, i);
            lua_pushvalue(L, argc);
            lua_call(L, 2, 1);
            luaL_addvalue(&b);
        }
        luaL_pushresult(&b);
        /* buffer ends here */
    }
    /* repr() should always return a string */
    return 1;
}

/**
 * @param: the values (0, 1 or more)
 * @return: the string
 */
static int l_compact_repr(lua_State * L)
{
    return l_temp_repr(L, l_compact_repr_slice);
}

/**
 * @param: the values (0, 1 or more)
 * @return: the string
 */
static int l_pretty_repr(lua_State * L)
{
    return l_temp_repr(L, l_pretty_repr_slice);
}

/**
 * @return: the registry
 */
static int l_getreg(lua_State * L)
{
    lua_pushvalue(L, LUA_REGISTRYINDEX);
    return 1;
}

/**
 * @return: the env
 */
static int l_getenv(lua_State * L)
{
    lua_pushvalue(L, LUA_ENVIRONINDEX);
    return 1;
}

/**
 * @arg1: file name
 * @return: table
 * this function has no exception handling, empty table is returned.
 */
static int l_readini(lua_State * L)
{
    const char *fname = luaL_checkstring(L, 1);
    lua_newtable(L);			/* +1 */
    FILE *fp = fopen(fname, "r");
    if (fp == NULL)
        return 1;
    char buf[BUFSIZ];
    char *result;
    /* marks the open state of the first section */
    bool section_opened = false;
    for (result = fgets(buf, BUFSIZ, fp); result != NULL; 
            result = fgets(buf, BUFSIZ, fp)) {
        char *begin, *end;
        /* check for a section */
        if (buf[0] == '[' && (end=strchr(buf, ']')) != NULL ) {
            end[0] = '\0';
            if (section_opened) {
                lua_settable(L, -3);	/* -2 */
            } else {
                section_opened = true;
            }
            /* save string between '[' and ']' */
            lua_pushstring(L, buf+1);	/* +1 */
            lua_newtable(L);		/* +1 */

        } else if ((end=strchr(buf, '=')) != NULL) {
            /* check for a key/value pair */
            /* check if '=' is at the first character */
            if (end == buf)
                continue;
            begin = buf;
            end[0] = '\0';	
            /* check and remove the redundant spaces */
            if (isspace(end[-1]))
                end[-1] = '\0';
            if (isspace(begin[0]))
                begin++;
            lua_pushstring(L, begin);	/* +1 */
            /* check and remove the redundant spaces */
            if (end[1] != '\0') {
                size_t last = strlen(end+1);
                if (isspace(end[last]))
                    end[last] = '\0';
                /* deal with the Ctrl-M */
                if (last > 0 && isspace(end[last-1]))
                    end[last-1] = '\0';
                if (isspace(end[1]))
                    end++;
            }
            lua_pushstring(L, end+1);	/* +1 */
            lua_settable(L, -3);	/* -2 */
        } else {
            /* ignored otherwise */
        }
    }
    if (section_opened) {
        lua_settable(L, -3);	/* -2 */
        section_opened = false;
    }

    return 1;
}

/**
 * @arg1: table
 * @arg2: file name
 * @return: none
 * this function has no exception handling, empty table is returned.
 */
static int l_writeini(lua_State * L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    const char *fname = luaL_checkstring(L, 2);
    FILE *fp = fopen(fname, "w");
    /* break if cannot write file */
    if (fp == NULL)
        return luaL_error(L, "cannot write file '%s'", fname);
    /* the first loop writes non-table data */
    for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
        /* key must be string, 
         * otherwise, tostring will change the key and break the loop */
        if (lua_type(L, -2) == LUA_TSTRING) {
            int tp = lua_type(L, -1);
            if (tp == LUA_TSTRING || tp == LUA_TNUMBER) {
                const char *value = lua_tostring(L, -1);
                const char *key = lua_tostring(L, -2);
                fprintf(fp, "%s=%s\n", key, value);
            }
        }
    }
    /* all non-table data must write before table data */
    fprintf(fp, "\n");
    /* the second loop writes table data */
    for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
        /* key is string, value is table */
        if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TTABLE) {
            const char *section = lua_tostring(L, -2);
            fprintf(fp, "[%s]\n", section);
            for (lua_pushnil(L); lua_next(L, 4) != 0; lua_pop(L, 1)) {
                if (lua_type(L, -2) == LUA_TSTRING) {
                    int tp = lua_type(L, -1);
                    if (tp == LUA_TSTRING || tp == LUA_TNUMBER) {
                        const char *value = lua_tostring(L, -1);
                        const char *key = lua_tostring(L, -2);
                        fprintf(fp, "%s=%s\n", key, value);
                    }
                }
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
    return 0;
}

/**
 * takes any args and always return nil
 */
static int l_nil(lua_State * L)
{
    lua_pushnil(L);
    return 1;
}

/* nil-funcs list */
static const char *l_nilfuncs[] = {
    "userdata_", "table_", "function_", "unknown_", "thread_",
    "ua", "te", "fn", "td"
};

/**
 * @arg1: the string to be unrepr
 * @return: multiple return, depends on input
 */
static int l_unrepr(lua_State * L)
{
    const char *expr = luaL_checkstring(L, 1);
    size_t len = strlen(expr);
    char buf[8+len];
    sprintf(buf, "return %s", expr);
    int result = luaL_loadstring(L, buf);
    if (result == LUA_ERRSYNTAX) {
        result = luaL_loadstring(L, expr);
    }
    if (result != 0) {
        /* error compiling the string, return nil */
        return 0;
    }
    int top = lua_gettop(L);
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result == 0) {
        /* return all the values */
        return lua_gettop(L) + 1 - top;
    } else {
        return 0;
    }
}

/**
 * @arg1: the file name
 * @arg2: the datas
 * @return: nothing
 */
static int l_writedata(lua_State *L)
{
    const char *fname = luaL_checkstring(L, 1);

    FILE *fp = fopen(fname, "w");
    if (fp == NULL) {
        /* open file error, return nil */
        return 0;
    }
    lua_pushcfunction(L, l_pretty_repr);
    lua_insert(L, 2);	/* push function into place */
    int nargs = lua_gettop(L) - 2;
    lua_call(L, nargs, 1);
    /* return in stack 2 */
    size_t len;
    const char *expr = lua_tolstring(L, 2, &len);
    fwrite(expr, 1, len, fp);
    fwrite("\n", 1, 1, fp);	/* ensure line end */
    fclose(fp);
    return 0;
}

/**
 * @arg: datas
 */
static int l_print_repr(lua_State *L)
{
    lua_pushcfunction(L, l_pretty_repr);
    lua_insert(L, 1);
    lua_call(L, lua_gettop(L)-1, 1);
    const char *expr = lua_tostring(L, -1);
    puts(expr);
    return 0;
}

/* keywords list */
static const char *l_keywords[] = {
    "and",       "break",     "do",        "else",      "elseif",
    "end",       "false",     "for",       "function",  "if",
    "in",        "local",     "nil",       "not",       "or",
    "repeat",    "return",    "then",      "true",      "until",     "while"
};

static const luaL_Reg toolslib[] = {
    { "bit_and", l_bitand },
    { "bit_not", l_bitnot },
    { "bit_or", l_bitor },
    { "bit_xor", l_bitxor },
    { "getenv", l_getenv },
    { "getreg", l_getreg },
    { "hexdump", l_hex_dump },
    { "prepr", l_print_repr },
    { "repr", l_pretty_repr },
    { "crepr", l_compact_repr },
    { "unrepr", l_unrepr },
    { "rlog", l_syslog },
    { "readini", l_readini },
    { "writeini", l_writeini },
    { "writedata", l_writedata },
    { "uptime", l_uptime },
    { NULL, NULL }
};

/**
 * Open tools library
 */
LUALIB_API int luaopen_libtools(lua_State * L)
{
    /* keywords table for l_repr_key() */
    size_t arraysize = sizeof l_keywords / sizeof l_keywords[0];
    lua_createtable(L, 0, arraysize);		/* +1 */
    for (size_t i = 0; i < arraysize; ++i) {
        lua_pushboolean(L, true);		/* +1 */
        lua_setfield(L, -2, l_keywords[i]);	/* -1 */
    }
    lua_setfield(L, LUA_REGISTRYINDEX, "tools.l_keywords");	/* -1 */

    /* prepare to save value to global */
    lua_pushvalue(L, LUA_GLOBALSINDEX);		/* +1 */

    /* register nil functions as global */
    arraysize = sizeof l_nilfuncs / sizeof l_nilfuncs[0];
    lua_pushcfunction(L, l_nil);		/* +1 */
    for (size_t i = 0; i < arraysize; ++i) {
        lua_pushstring(L, l_nilfuncs[i]);	/* +1 */
        lua_pushvalue(L, -2);			/* +1 */
        lua_rawset(L, LUA_GLOBALSINDEX);	/* -2 */
    }
    lua_pop(L, 1);				/* -1 */

    /* register functions in toolslib array */
    if (REGISTER_GLOBAL) {
        luaL_register(L, NULL, toolslib);	/* 0 */
        lua_pop(L, 1);				/* -1 */
        lua_createtable(L, 0, arraysize);	/* +1 */
        luaL_register(L, NULL, toolslib);	/* 0 */
    } else {
        lua_pop(L, 1);				/* -1 */
        luaL_register(L, "tools", toolslib);	/* +1 for createtable */
    }

    /* return the last created table, which may create libtools.* */
    return 1;
}

/* allow user to use tools.so instead of libtools.so */
LUALIB_API int luaopen_tools(lua_State * L)
{
    return luaopen_libtools(L);
}

