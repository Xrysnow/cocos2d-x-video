#include "lua_video_auto.hpp"
#include "VideoPlayer.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

int lua_cc_video_Player_saveCurrentFrame(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_saveCurrentFrame'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "video.Player:saveCurrentFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_saveCurrentFrame'", nullptr);
            return 0;
        }
        cobj->saveCurrentFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:saveCurrentFrame", argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_saveCurrentFrame'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_setAutoRemove(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_setAutoRemove'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "video.Player:setAutoRemove");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_setAutoRemove'", nullptr);
            return 0;
        }
        cobj->setAutoRemove(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:setAutoRemove", argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_setAutoRemove'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_setStep(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_setStep'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "video.Player:setStep");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_setStep'", nullptr);
            return 0;
        }
        cobj->setStep(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:setStep", argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_setStep'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_vpause(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_vpause'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_vpause'", nullptr);
            return 0;
        }
        cobj->vpause();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:vpause", argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_vpause'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_setPlayMode(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_setPlayMode'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) 
    {
        video::Player::PlayMode arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "video.Player:setPlayMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_setPlayMode'", nullptr);
            return 0;
        }
        cobj->setPlayMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:setPlayMode", argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_setPlayMode'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_vplay(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_vplay'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_vplay'", nullptr);
            return 0;
        }
        cobj->vplay();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:vplay", argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_vplay'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_vresume(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_vresume'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_vresume'", nullptr);
            return 0;
        }
        cobj->vresume();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:vresume", argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_vresume'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_vstop(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_vstop'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_vstop'", nullptr);
            return 0;
        }
        cobj->vstop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:vstop", argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_vstop'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_seek(lua_State* tolua_S)
{
    int argc = 0;
    video::Player* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cc_video_Player_seek'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "video.Player:seek");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_seek'", nullptr);
            return 0;
        }
        cobj->seek(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "video.Player:seek", argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_seek'.",&tolua_err);
    return 0;
#endif
}
int lua_cc_video_Player_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"video.Player",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "video.Player:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cc_video_Player_create'", nullptr);
            return 0;
        }
        video::Player* ret = video::Player::create(arg0);
        object_to_luaval<video::Player>(tolua_S, "video.Player",(video::Player*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "video.Player:create",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cc_video_Player_create'.",&tolua_err);
#endif
    return 0;
}

int lua_register_cc_video_Player(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"video.Player");
    tolua_cclass(tolua_S,"Player","video.Player","cc.Sprite",nullptr);

    tolua_beginmodule(tolua_S,"Player");
        tolua_function(tolua_S,"saveCurrentFrame",lua_cc_video_Player_saveCurrentFrame);
        tolua_function(tolua_S,"setAutoRemove",lua_cc_video_Player_setAutoRemove);
        tolua_function(tolua_S,"setStep",lua_cc_video_Player_setStep);
        tolua_function(tolua_S,"vpause",lua_cc_video_Player_vpause);
        tolua_function(tolua_S,"setPlayMode",lua_cc_video_Player_setPlayMode);
        tolua_function(tolua_S,"vplay",lua_cc_video_Player_vplay);
        tolua_function(tolua_S,"vresume",lua_cc_video_Player_vresume);
        tolua_function(tolua_S,"vstop",lua_cc_video_Player_vstop);
        tolua_function(tolua_S,"seek",lua_cc_video_Player_seek);
        tolua_function(tolua_S,"create", lua_cc_video_Player_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(video::Player).name();
    g_luaType[typeName] = "video.Player";
    g_typeCast["Player"] = "video.Player";
    return 1;
}
TOLUA_API int register_all_cc_video(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"video",0);
	tolua_beginmodule(tolua_S,"video");

	lua_register_cc_video_Player(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

