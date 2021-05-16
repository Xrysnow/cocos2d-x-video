#include "lua_video_auto.hpp"
#include "VideoPlayer.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

#ifndef LUA_CHECK_COBJ_TYPE
	#ifdef LUA_DEBUG
		#define LUA_CHECK_COBJ_TYPE(L, TYPE, NAME) if(!tolua_isusertype((L), 1, (TYPE), 0, nullptr)) { return luaL_error((L), "invalid 'cobj' in '%s': '%s', expects '%s'", NAME, tolua_typename((L), 1), (TYPE)); }
	#else
		#define LUA_CHECK_COBJ_TYPE(L, TYPE, NAME) (void)(TYPE);
	#endif
#endif
#ifndef LUA_CHECK_COBJ
	#ifdef LUA_DEBUG
		#define LUA_CHECK_COBJ(L, COBJ, NAME) if(!(COBJ)) { return luaL_error((L), "invalid 'cobj' in '%s'", NAME); }
	#else
		#define LUA_CHECK_COBJ(L, COBJ, NAME)
	#endif
#endif
#ifndef LUA_CHECK_PARAMETER
	#define LUA_CHECK_PARAMETER(L, OK, NAME) if(!(OK)) { return luaL_error((L), "invalid arguments in '%s'", NAME); }
#endif
#ifndef LUA_PARAMETER_ERROR
	#define LUA_PARAMETER_ERROR(L, NAME, ARGC, EXPECT) return luaL_error((L), "wrong number of arguments in '%s': %d, expects %s", NAME, (ARGC), EXPECT);
#endif

void AVRational_to_luaval(lua_State* L, const AVRational& value)
{
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, value.num);
	lua_setfield(L, -2, "num");
	lua_pushinteger(L, value.den);
	lua_setfield(L, -2, "den");
}

int lua_cc_video_Decoder_getAllDecoderNames(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAllDecoderNames";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::getAllDecoderNames();
		ccvector_std_string_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getAudioBitRate(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioBitRate";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioBitRate();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioBitRate(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getAudioChannelCount(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioChannelCount";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioChannelCount();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioChannelCount(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getAudioChannelLayoutName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioChannelLayoutName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioChannelLayoutName();
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioChannelLayoutName(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getAudioDecoderName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioDecoderName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioDecoderName();
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getAudioFrameCount(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioFrameCount";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioFrameCount();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioFrameCount(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getAudioSampleRate(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioSampleRate";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioSampleRate();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioSampleRate(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getAudioStreamCount(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioStreamCount";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioStreamCount();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getAudioTimeBase(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getAudioTimeBase";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioTimeBase();
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getAudioTimeBase(arg0);
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getContainerBitRate(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getContainerBitRate";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getContainerBitRate();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getContainerDuration(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getContainerDuration";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getContainerDuration();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getContainerStartTime(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getContainerStartTime";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getContainerStartTime();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getPixelFormatName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getPixelFormatName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		AVPixelFormat arg0;
		int arg0_tmp; ok &= luaval_to_int32(tolua_S, 2, &arg0_tmp, LUA_FNAME); arg0 = (AVPixelFormat)arg0_tmp;
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::getPixelFormatName(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Decoder_getVideoBitRate(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoBitRate";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoBitRate();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoBitRate(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoDecoderHardwareFormat(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoDecoderHardwareFormat";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoDecoderHardwareFormat();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getVideoDecoderName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoDecoderName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoDecoderName();
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getVideoDecoderSoftwareFormat(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoDecoderSoftwareFormat";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoDecoderSoftwareFormat();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getVideoFormat(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoFormat";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFormat();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFormat(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoFormatName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoFormatName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFormatName();
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFormatName(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoFrameCount(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoFrameCount";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFrameCount();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFrameCount(arg0);
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoFrameRate(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoFrameRate";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFrameRate();
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoFrameRate(arg0);
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoSize(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoSize";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoSize();
		size_to_luaval(tolua_S, ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoSize(arg0);
		size_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_getVideoStreamCount(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoStreamCount";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoStreamCount();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getVideoTargetSize(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoTargetSize";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoTargetSize();
		size_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_getVideoTimeBase(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:getVideoTimeBase";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoTimeBase();
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	if (argc == 1) {
		int arg0;
		ok &= luaval_to_int32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getVideoTimeBase(arg0);
		AVRational_to_luaval(tolua_S, ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0 to 1");
}
int lua_cc_video_Decoder_isVideoDecoderHardware(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:isVideoDecoderHardware";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->isVideoDecoderHardware();
		tolua_pushboolean(tolua_S, (bool)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Decoder_queryDecoderID(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:queryDecoderID";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::queryDecoderID(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Decoder_queryDecoderLongName(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:queryDecoderLongName";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::queryDecoderLongName(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Decoder_queryDecoderSupportsHardware(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:queryDecoderSupportsHardware";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::queryDecoderSupportsHardware(arg0);
		tolua_pushboolean(tolua_S, (bool)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Decoder_queryDecoderType(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:queryDecoderType";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Decoder::queryDecoderType(arg0);
		lua_pushlstring(tolua_S, ret.c_str(), ret.length());
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Decoder_tell(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Decoder";
	constexpr auto LUA_FNAME = "video.Decoder:tell";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Decoder*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->tell();
		tolua_pushnumber(tolua_S, (lua_Number)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
static int lua_cc_video_Decoder_finalize(lua_State* tolua_S)
{
	return 0;
}

int lua_register_cc_video_Decoder(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "video.Decoder");
	tolua_cclass(tolua_S, "Decoder", "video.Decoder", "cc.Ref", nullptr);

	tolua_beginmodule(tolua_S, "Decoder");
		tolua_function(tolua_S, "getAudioBitRate", lua_cc_video_Decoder_getAudioBitRate);
		tolua_function(tolua_S, "getAudioChannelCount", lua_cc_video_Decoder_getAudioChannelCount);
		tolua_function(tolua_S, "getAudioChannelLayoutName", lua_cc_video_Decoder_getAudioChannelLayoutName);
		tolua_function(tolua_S, "getAudioDecoderName", lua_cc_video_Decoder_getAudioDecoderName);
		tolua_function(tolua_S, "getAudioFrameCount", lua_cc_video_Decoder_getAudioFrameCount);
		tolua_function(tolua_S, "getAudioSampleRate", lua_cc_video_Decoder_getAudioSampleRate);
		tolua_function(tolua_S, "getAudioStreamCount", lua_cc_video_Decoder_getAudioStreamCount);
		tolua_function(tolua_S, "getAudioTimeBase", lua_cc_video_Decoder_getAudioTimeBase);
		tolua_function(tolua_S, "getContainerBitRate", lua_cc_video_Decoder_getContainerBitRate);
		tolua_function(tolua_S, "getContainerDuration", lua_cc_video_Decoder_getContainerDuration);
		tolua_function(tolua_S, "getContainerStartTime", lua_cc_video_Decoder_getContainerStartTime);
		tolua_function(tolua_S, "getVideoBitRate", lua_cc_video_Decoder_getVideoBitRate);
		tolua_function(tolua_S, "getVideoDecoderHardwareFormat", lua_cc_video_Decoder_getVideoDecoderHardwareFormat);
		tolua_function(tolua_S, "getVideoDecoderName", lua_cc_video_Decoder_getVideoDecoderName);
		tolua_function(tolua_S, "getVideoDecoderSoftwareFormat", lua_cc_video_Decoder_getVideoDecoderSoftwareFormat);
		tolua_function(tolua_S, "getVideoFormat", lua_cc_video_Decoder_getVideoFormat);
		tolua_function(tolua_S, "getVideoFormatName", lua_cc_video_Decoder_getVideoFormatName);
		tolua_function(tolua_S, "getVideoFrameCount", lua_cc_video_Decoder_getVideoFrameCount);
		tolua_function(tolua_S, "getVideoFrameRate", lua_cc_video_Decoder_getVideoFrameRate);
		tolua_function(tolua_S, "getVideoSize", lua_cc_video_Decoder_getVideoSize);
		tolua_function(tolua_S, "getVideoStreamCount", lua_cc_video_Decoder_getVideoStreamCount);
		tolua_function(tolua_S, "getVideoTargetSize", lua_cc_video_Decoder_getVideoTargetSize);
		tolua_function(tolua_S, "getVideoTimeBase", lua_cc_video_Decoder_getVideoTimeBase);
		tolua_function(tolua_S, "isVideoDecoderHardware", lua_cc_video_Decoder_isVideoDecoderHardware);
		tolua_function(tolua_S, "tell", lua_cc_video_Decoder_tell);
		tolua_function(tolua_S, "getAllDecoderNames", lua_cc_video_Decoder_getAllDecoderNames);
		tolua_function(tolua_S, "getPixelFormatName", lua_cc_video_Decoder_getPixelFormatName);
		tolua_function(tolua_S, "queryDecoderID", lua_cc_video_Decoder_queryDecoderID);
		tolua_function(tolua_S, "queryDecoderLongName", lua_cc_video_Decoder_queryDecoderLongName);
		tolua_function(tolua_S, "queryDecoderSupportsHardware", lua_cc_video_Decoder_queryDecoderSupportsHardware);
		tolua_function(tolua_S, "queryDecoderType", lua_cc_video_Decoder_queryDecoderType);
	tolua_endmodule(tolua_S);
	std::string typeName = typeid(video::Decoder).name();
	g_luaType[typeName] = "video.Decoder";
	g_typeCast["Decoder"] = "video.Decoder";
	return 1;
}

int lua_cc_video_Player_create(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:create";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = video::Player::create(arg0);
		object_to_luaval<video::Player>(tolua_S, "video.Player", (video::Player*)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_getDecoder(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:getDecoder";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		auto ret = cobj->getDecoder();
		object_to_luaval<video::Decoder>(tolua_S, "video.Decoder", (video::Decoder*)ret);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Player_saveCurrentFrame(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:saveCurrentFrame";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::string arg0;
		ok &= luaval_to_std_string(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->saveCurrentFrame(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_seek(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:seek";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		unsigned int arg0;
		ok &= luaval_to_uint32(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->seek(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_setAutoRemove(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:setAutoRemove";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		bool arg0;
		ok &= luaval_to_boolean(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->setAutoRemove(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_setPlayMode(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:setPlayMode";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		video::Player::PlayMode arg0;
		int arg0_tmp; ok &= luaval_to_int32(tolua_S, 2, &arg0_tmp, LUA_FNAME); arg0 = (video::Player::PlayMode)arg0_tmp;
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->setPlayMode(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_setStep(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:setStep";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		double arg0;
		ok &= luaval_to_number(tolua_S, 2, &arg0, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->setStep(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_setVideoEndCallback(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:setVideoEndCallback";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 1) {
		std::function<void ()> arg0;
		int handler2 = toluafix_ref_function(tolua_S, 2, 0);
		ok &= handler2 != 0;
		if(ok)
		{
			arg0 = [=]()
			{
				toluafix_get_function_by_refid(tolua_S, handler2);
				if (!lua_isfunction(tolua_S, -1))
				{
					lua_pop(tolua_S, 1);
					return ;
				}
				lua_call(tolua_S, 0, 0);
			};
		};
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->setVideoEndCallback(arg0);
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
int lua_cc_video_Player_vpause(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:vpause";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->vpause();
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Player_vplay(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:vplay";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->vplay();
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Player_vresume(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:vresume";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->vresume();
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
int lua_cc_video_Player_vstop(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "video.Player";
	constexpr auto LUA_FNAME = "video.Player:vstop";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (video::Player*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (argc == 0) {
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->vstop();
		lua_settop(tolua_S, 1);
		return 1;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "0");
}
static int lua_cc_video_Player_finalize(lua_State* tolua_S)
{
	return 0;
}

int lua_register_cc_video_Player(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "video.Player");
	tolua_cclass(tolua_S, "Player", "video.Player", "cc.Sprite", nullptr);

	tolua_beginmodule(tolua_S, "Player");
		tolua_function(tolua_S, "getDecoder", lua_cc_video_Player_getDecoder);
		tolua_function(tolua_S, "saveCurrentFrame", lua_cc_video_Player_saveCurrentFrame);
		tolua_function(tolua_S, "seek", lua_cc_video_Player_seek);
		tolua_function(tolua_S, "setAutoRemove", lua_cc_video_Player_setAutoRemove);
		tolua_function(tolua_S, "setPlayMode", lua_cc_video_Player_setPlayMode);
		tolua_function(tolua_S, "setStep", lua_cc_video_Player_setStep);
		tolua_function(tolua_S, "setVideoEndCallback", lua_cc_video_Player_setVideoEndCallback);
		tolua_function(tolua_S, "vpause", lua_cc_video_Player_vpause);
		tolua_function(tolua_S, "vplay", lua_cc_video_Player_vplay);
		tolua_function(tolua_S, "vresume", lua_cc_video_Player_vresume);
		tolua_function(tolua_S, "vstop", lua_cc_video_Player_vstop);
		tolua_function(tolua_S, "create", lua_cc_video_Player_create);
		{
			tolua_module(tolua_S, "PlayMode", 0);
			tolua_beginmodule(tolua_S, "PlayMode");
			{
				tolua_constant(tolua_S, "STEP", (lua_Number)video::Player::PlayMode::STEP);
				tolua_constant(tolua_S, "REALTIME", (lua_Number)video::Player::PlayMode::REALTIME);
				tolua_constant(tolua_S, "FRAME", (lua_Number)video::Player::PlayMode::FRAME);
				tolua_constant(tolua_S, "MANUAL", (lua_Number)video::Player::PlayMode::MANUAL);
			}
			tolua_endmodule(tolua_S);
		}
	tolua_endmodule(tolua_S);
	std::string typeName = typeid(video::Player).name();
	g_luaType[typeName] = "video.Player";
	g_typeCast["Player"] = "video.Player";
	return 1;
}

int register_all_cc_video(lua_State* tolua_S)
{
	tolua_open(tolua_S);

	tolua_module(tolua_S, "video", 0);
	tolua_beginmodule(tolua_S, "video");

	lua_register_cc_video_Player(tolua_S);
	lua_register_cc_video_Decoder(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

