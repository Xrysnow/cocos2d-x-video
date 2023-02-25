set(LIB_NAME cc_video)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COCOS2DX_ROOT_PATH}/cocos
    ${COCOS2DX_ROOT_PATH}/cocos/base
    ${COCOS2DX_ROOT_PATH}/cocos/platform
    ${COCOS2DX_ROOT_PATH}/extensions
    ${COCOS2DX_ROOT_PATH}/external
    ${COCOS2DX_ROOT_PATH}/external/glfw3/include/win32
    ${COCOS2DX_ROOT_PATH}/external/glfw/include/GLFW
    ${COCOS2DX_ROOT_PATH}/external/win32-specific/gles/include/OGLES
)
if(BUILD_LUA_LIBS)
    list(APPEND INCLUDES
        ${COCOS2DX_ROOT_PATH}/external/lua/luajit/include
        ${COCOS2DX_ROOT_PATH}/external/lua/tolua)
endif()
foreach(dir ${INCLUDES})
    if(EXISTS ${dir})
        include_directories(${dir})
    endif()
endforeach()

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
use_cocos2dx_compile_options(${LIB_NAME})

if(CC_VIDEO_DEPEND_LIBS)
    target_link_libraries(${LIB_NAME} ${CC_VIDEO_DEPEND_LIBS})
endif()

if(BUILD_LUA_LIBS)
    target_compile_definitions(${LIB_NAME} PUBLIC _USRLUASTATIC)
endif()

if(FFMPEG_LIBRARY)
    #target_link_libraries(${LIB_NAME} ${FFMPEG_LIBRARY})
    #set_target_properties(${LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS ${FFMPEG_FRAMEWORK_PATH})
    target_include_directories(${LIB_NAME} PUBLIC FFMPEG_INCLUDE_DIR)
endif()

set_target_properties(${LIB_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)

if(XCODE OR VS)
    cocos_mark_code_files(${LIB_NAME})
endif()
