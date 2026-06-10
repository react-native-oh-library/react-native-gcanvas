cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)
set(GCANVAS_WEEX ON)
cmake_policy(SET CMP0152 NEW)

set(rnoh_gcanvas_generated_dir "${CMAKE_CURRENT_SOURCE_DIR}/generated")
file(GLOB_RECURSE rnoh_gcanvas_generated_SRC "${rnoh_gcanvas_generated_dir}/**/*.cpp")
file(GLOB rnoh_gcanvas_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_gcanvas SHARED ${rnoh_gcanvas_SRC} ${rnoh_gcanvas_generated_SRC})

target_include_directories(rnoh_gcanvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_gcanvas_generated_dir})

if (GCANVAS_WEEX)
    target_compile_definitions(rnoh_gcanvas PRIVATE GCANVAS_WEEX)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/base64/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/base64)

if(TARGET base64)
    # 设置C++编译选项
    target_compile_options(base64 PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-command-line-argument>
    )

    # 设置C编译选项
    target_compile_options(base64 PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-command-line-argument>
    )
endif()

add_subdirectory(core)

target_link_libraries(rnoh_gcanvas PUBLIC rnoh
                                          gcanvas
                                          libace_ndk.z.so
                                          libhilog_ndk.z.so
                                          libnative_drawing.so
                                          libnative_window.so
                                          base64
                                          libimage_packer.so
#                                          ${EGL-lib} ${GLES-lib}
                                          )

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../oh_modules/@ohos/imageknifepro")
    set(IMAGEKNIFEPRO_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../oh_modules/@ohos/imageknifepro)
    message("[GCnavas]: rnt")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules/@ohos/imageknifepro")
    set(IMAGEKNIFEPRO_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules/@ohos/imageknifepro)
    message("[GCnavas]: rn")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../oh_modules/.ohpm/oh_modules/@ohos/imageknifepro")
    set(IMAGEKNIFEPRO_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../oh_modules/.ohpm/oh_modules/@ohos/imageknifepro)
    message("[GCnavas]: rn")
else()
    set(IMAGEKNIFEPRO_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../oh_modules/.ohpm/oh_modules/@ohos/imageknifepro)
    message("[GCnavas]: ohpm")
endif()

file(REAL_PATH "${IMAGEKNIFEPRO_ROOT_PATH}" real)
set(IMAGEKNIFEPRO_ROOT_PATH "${real}")
set(CMAKE_MODULE_PATH ${real})

find_package(ImageKnifePro REQUIRED)

target_link_libraries(rnoh_gcanvas PUBLIC Imageknifepro::libimageknifepro)