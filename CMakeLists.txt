set(LIB_NAME cc_video)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COCOS2DX_ROOT_PATH}/cocos
    ${COCOS2DX_ROOT_PATH}/cocos/base
    ${COCOS2DX_ROOT_PATH}/cocos/platform
    ${COCOS2DX_ROOT_PATH}/external
    ${COCOS2DX_ROOT_PATH}/external/glfw3/include/win32
)

if(WINDOWS)
    include_directories(${COCOS2DX_ROOT_PATH}/external/win32-specific/gles/include/OGLES)
endif()

set(HEADER
    FFCodec.h
    FFDemuxer.h
    FFFrameQueue.h
    FFFrameReceiver.h
    FFHardware.h
    FFInfo.h
    FFInputStream.h
    FFPointer.h
    VideoCommon.h
    VideoDecoder.h
    VideoPlayer.h
    VideoStream.h
)

set(SOURCE
    FFCodec.cpp
    FFDemuxer.cpp
    FFFrameQueue.cpp
    FFHardware.cpp
    FFInputStream.cpp
    VideoCommon.cpp
    VideoDecoder.cpp
    VideoPlayer.cpp
)

if(BUILD_LUA_LIBS)
    include_directories(
        lua-bindings
        ${COCOS2DX_ROOT_PATH}/external/lua/luajit/include
        ${COCOS2DX_ROOT_PATH}/external/lua/tolua
    )
    list(APPEND HEADER
        lua_video_auto.hpp
    )
    list(APPEND SOURCE
        lua_video_auto.cpp
    )
endif()

add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADER})
add_dependencies(${LIB_NAME} cocos2d)
use_cocos2dx_compile_define(${LIB_NAME})

if(CC_VIDEO_DEPEND_LIBS)
    target_link_libraries(${LIB_NAME} ${CC_VIDEO_DEPEND_LIBS})
endif()

if(BUILD_LUA_LIBS)
    target_compile_definitions(${LIB_NAME} PUBLIC _USRLUASTATIC)
endif()

set_target_properties(${LIB_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(XCODE OR VS)
    cocos_mark_code_files(${LIB_NAME})
endif()
