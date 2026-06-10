# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-exceptions -fno-unwind-tables")

add_definitions(-DFT2_BUILD_LIBRARY)
add_definitions(-DFT2_PNG_RGBA_NOT_BGRA)
add_definitions(-DOHOS)

#if ((${ANDROID_ARM_NEON}) AND (${ANDROID_ABI} MATCHES arm*))
#    add_definitions(-DARM_NEON_OPT)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=neon")
#endif ()
#
#
include_directories("./src/")
include_directories("./src/gcanvas")
include_directories("./src/support")
include_directories("./src/platform/ohos")
include_directories("./harmony/")
include_directories("./harmony/png")
include_directories("./harmony/png/thirdparty")
include_directories("./harmony/memory")
include_directories("./harmony/3d")
include_directories("./harmony/3d/jsc")
include_directories("./harmony/3d/util")
include_directories("./harmony/3d/view")
include_directories("./harmony/freetype/include")


set(SRC_FILES
        #root srcs
        ./src/GCanvas.cpp
        ./src/GCanvasManager.cpp
        ./src/GCanvasWeex.cpp # todo remove

        # gcanvas srcs
        ./src/gcanvas/GCanvas2dContext.cpp
        ./src/gcanvas/GCanvasState.cpp
        ./src/gcanvas/GConvert.cpp
        ./src/gcanvas/GFontStyle.cpp
        ./src/gcanvas/GFrameBufferObject.cpp
        ./src/gcanvas/GGlyphCache.cpp
        ./src/gcanvas/GPath.cpp
        ./src/gcanvas/GShader.cpp
        ./src/gcanvas/GShaderManager.cpp
        ./src/gcanvas/GStrSeparator.cpp
        ./src/gcanvas/GTexture.cpp
        ./src/gcanvas/GTreemap.cpp
        ./src/gcanvas/GWebglContext.cpp

        # platform srcs
        ./src/platform/ohos/GCanvas2DContextOHOS.cpp

        ./src/platform/ohos/GCanvas2DContextHarmony.cpp
        ./src/platform/ohos/GCanvasHarmony.cpp
        ./src/platform/ohos/GFont.cpp
        ./src/platform/ohos/GFontCache.cpp
        ./src/platform/ohos/GFontFamily.cpp
        ./src/platform/ohos/GFontManagerHarmony.cpp
        ./src/platform/ohos/GFontManagerImpl.cpp

        ./src/platform/ohos/GFrameBufferObjectImpl.cpp

        ./src/platform/ohos/GPreCompiledShaders.cpp
        ./src/platform/ohos/GSystemFontInformation.cpp


         #support
        ./src/support/CharacterSet.cpp
        ./src/support/Encode.cpp
        ./src/support/FileUtils.cpp
        ./src/support/GLUtil.cpp
        ./src/support/Log.cpp
        ./src/support/Util.cpp
        ./src/support/Value.cpp
)

message(" not enter the gcanvas_weex >1>>>>>>>>>:${GCANVAS_WEEX}")
if (GCANVAS_WEEX)
    add_definitions(-DGCANVAS_WEEX)
    message(" not enter the gcanvas_weex >>>>>>>>>> ")
    file(GLOB ohos_root_src "./harmony/*.cpp")
    file(GLOB native_png_src "./harmony/png/*.cpp")
    file(GLOB native_png_thirdparty_src "./harmony/png/thirdparty/*.c")
    file(GLOB native_3d_src "./harmony/3d/*.cpp")
    file(GLOB native_3d_jsc_src "./harmony/3d/jsc/*.cpp")
    file(GLOB native_3d_util_src "./harmony/3d/util/*.cpp")
    file(GLOB native_3d_view_src "./harmony/3d/view/*.cpp")
    file(GLOB native_manager_src "./harmony/manager/*.cpp")
#else()
#    message(" not enter the gcanvas_weex >>>>>>>>>> ")
#    list(APPEND SRC_FILES ./android/GCanvasJNI.cpp)
endif()


if (GCANVAS_RUNTIME)
    add_definitions(-DGCANVAS_RUNTIME)
endif()

add_library( # Sets the name of the library.
        gcanvas
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${native_png_src}
        ${native_png_thirdparty_src}
        ${native_3d_src}
        ${native_3d_jsc_src}
        ${native_3d_util_src}
        ${native_3d_view_src}
        ${ohos_root_src}
        ${native_manager_src}
        ${SRC_FILES}
        )


target_include_directories(gcanvas PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/gcanvas
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/support
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/ohos
                            ${CMAKE_CURRENT_SOURCE_DIR}/harmony/3d
                            ${CMAKE_CURRENT_SOURCE_DIR}/harmony/3d/jsc
                            ${CMAKE_CURRENT_SOURCE_DIR}/harmony/3d/util
                            ${CMAKE_CURRENT_SOURCE_DIR}/harmony/3d/view
                            )
set_target_properties(gcanvas PROPERTIES CXX_VISIBILITY_PRESET default)
# freetype v2.13.3
set(FREETYPE_SRC_FILES
        ./harmony/freetype/src/autofit/autofit.c
        ./harmony/freetype/src/base/ftbase.c
        ./harmony/freetype/src/base/ftbbox.c
        ./harmony/freetype/src/base/ftbitmap.c
        ./harmony/freetype/src/base/ftdbgmem.c
        ./harmony/freetype/src/base/ftdebug.c
        ./harmony/freetype/src/base/ftglyph.c
        ./harmony/freetype/src/base/ftinit.c
        ./harmony/freetype/src/base/ftmm.c
        ./harmony/freetype/src/base/ftstroke.c
        ./harmony/freetype/src/base/ftsynth.c
        ./harmony/freetype/src/base/ftsystem.c
        ./harmony/freetype/src/bdf/bdf.c
        ./harmony/freetype/src/cid/type1cid.c
        ./harmony/freetype/src/cff/cff.c
        ./harmony/freetype/src/gzip/ftgzip.c
        ./harmony/freetype/src/lzw/ftlzw.c
        ./harmony/freetype/src/pcf/pcf.c
        ./harmony/freetype/src/pfr/pfr.c
        ./harmony/freetype/src/psaux/psaux.c
        ./harmony/freetype/src/pshinter/pshinter.c
        ./harmony/freetype/src/psnames/psnames.c
        ./harmony/freetype/src/raster/raster.c
        ./harmony/freetype/src/sdf/ftbsdf.c
        ./harmony/freetype/src/sdf/ftsdf.c
        ./harmony/freetype/src/sdf/ftsdfcommon.c
        ./harmony/freetype/src/sdf/ftsdfrend.c
        ./harmony/freetype/src/sfnt/sfnt.c
        ./harmony/freetype/src/smooth/smooth.c
        ./harmony/freetype/src/svg/svg.c
        ./harmony/freetype/src/truetype/truetype.c
        ./harmony/freetype/src/type1/type1.c
        ./harmony/freetype/src/type42/type42.c
        ./harmony/freetype/src/winfonts/winfnt.c )

add_library(freetype SHARED ${FREETYPE_SRC_FILES})


# ref to [How to integrate libpng in my own project's CMakeLists.txt](https://github.com/pnggroup/libpng/issues/342#issuecomment-864589614)

# libpng configurable options

set(PNG_SHARED OFF CACHE BOOL "" FORCE)  # 关闭动态库
set(PNG_STATIC ON CACHE BOOL "" FORCE)   # 开启静态库
set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE) # 关闭安装规则，避免生成 symlink

# add libpng

include_directories(./harmony/libpng)
# generated files from libpng during build
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libpng)

add_subdirectory(./harmony/libpng)


target_link_libraries( # Specifies the target library.
        gcanvas
#        GLESv1_CM
        GLESv3
#        android
        EGL
        GLESv2
#        atomic
#        jnigraphics
        freetype
#        # Links the target library to the log library
#        # included in the NDK.
#        ${log-lib}
        libhilog_ndk.z.so
        libnative_window.so
        libicu.so
        libimage_source.so libpixelmap.so
        )

target_link_options(
        gcanvas
        PRIVATE
        "-Wl,-z,max-page-size=16384"
)


# here use ${log-lib} to support logcat with e.g. LOGD in freetype source code by
# add below #define code into ./android/freetype/include/freetype/internal/ftdebug.h

target_link_libraries(
        freetype
        png_static
        )

target_link_options(
        freetype
        PRIVATE
        "-Wl,-z,max-page-size=16384"
)
