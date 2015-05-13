#include <ctype.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define MT_NUM 624

typedef struct Mt{
    int seed;
    int index;
    long long mtarray[MT_NUM];
} Mt;

static int l_setseed(lua_State *L){
    Mt *mt;
    int i;
    char s[] = "setseed";
    if (!lua_istable(L, 1)){
        luaL_argerror(L, 1, s);
        return 0;
    }
    lua_getfield(L, 1, "__mt");
    mt = (Mt *)lua_touserdata(L, -1);
    mt->seed = lua_tointeger(L, 2);
    mt->index = 0;
    mt->mtarray[0] = mt->seed;
    for (i = 1; i < MT_NUM; i++){
        mt->mtarray[i] = (1812433253 * (mt->mtarray[i-1] ^ ((mt->mtarray[i-1] >> 30))) + i) & 0xffffffff;
    }
    lua_pushnil(L);
    return 1;
}

static int l_initmt(lua_State *L){
    char s[] = "initmt";
    Mt *mt = (Mt *)lua_newuserdata(L, sizeof(Mt));
    if (!lua_istable(L, 1)){
        luaL_argerror(L, 1, s);
        return 0;
    }
    lua_pushstring(L, "__mt");
    lua_pushlightuserdata(L, mt);
    lua_settable(L, 1);
    l_setseed(L);
    lua_pushnil(L);
    return 1;
}

static int l_generatenum(lua_State *L){
    int i;
    long long y;
    Mt *mt;
    lua_getfield(L, 1, "__mt");
    mt = (Mt *)lua_touserdata(L, -1);
    for (i = 0; i < MT_NUM; i++){
        y = (mt->mtarray[i] & 0x80000000) + (mt->mtarray[(i+1) % MT_NUM] & 0x7fffffff);
        mt->mtarray[i] = mt->mtarray[(i + 397) % MT_NUM] ^ (y >> 1);
        if (y % 2 != 0)
            mt->mtarray[i] = mt->mtarray[i] ^ (2567483615);
    }
    lua_pushnil(L);
    return 1;
}

static int l_random(lua_State *L){
    char s[] = "random";
    long long y;
    int start, end;
    Mt *mt;
    if (!lua_istable(L, 1)){
        luaL_argerror(L, 1, s);
        return 0;
    }
    lua_getfield(L, 1, "__mt");
    mt = (Mt *)lua_touserdata(L, -1);
    if (mt->index == 0)
        l_generatenum(L);
    start = lua_tointeger(L, 2);
    end = lua_tointeger(L, 3);
    if (end <= 0){
        end = start;
        start = 1;
    }
    luaL_argcheck(L, start <= end, 2, "arg #3 must >= #2");
    y = mt->mtarray[mt->index];
    y = y ^ (y >> 11);
    y = y ^ (y << 7) & 2636928640;
    y = y ^ (y << 15) & 4022730752;
    y = y ^ (y >> 18);
    y = start + y % (end - start + 1);
    mt->index = (mt->index + 1) % MT_NUM;
    lua_pushinteger(L, y);
    return 1;
}

static int l_getseed(lua_State *L){
    Mt *mt;
    char s[] = "getseed";
    if (!lua_istable(L, 1)){
        luaL_argerror(L, 1, s);
        return 0;
    }
    lua_getfield(L, 1, "__mt");
    mt = (Mt *)lua_touserdata(L, -1);
    lua_pushinteger(L, mt->seed);
    return 1;
}

static const luaL_Reg randomlib[] = {
    {"initmt", l_initmt},
    {"random",  l_random},
    {"getseed", l_getseed},
    {"setseed", l_setseed},
    {NULL,  NULL},
};

LUAMOD_API int luaopen_lrandom(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, randomlib, 0);
    return 1;
}
