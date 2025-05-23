cmake_minimum_required(VERSION 3.17.0)
project(Mosaify VERSION 1.0.7)

# Testing Mosaify target
include(FetchContent)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "My Comment" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "/System/Volumes/Data/usr/local/Cellar/cmake/3.27.7/share/cmake/Modules/")


add_subdirectory(glm)

add_subdirectory(json)
#FetchContent_Declare(
#        json
#        GIT_REPOSITORY https://github.com/nlohmann/json.git
#        GIT_TAG v3.12.0
#)
#FetchContent_MakeAvailable(json)

#find_package(ImageMagick REQUIRED COMPONENTS Magick++)
#add_subdirectory(libboard)
#FetchContent_Declare(
#        libboard
#        GIT_REPOSITORY https://github.com/c-koi/libboard.git
#        GIT_TAG v0.9.6
#)
#FetchContent_MakeAvailable(libboard)

add_library(Mosaify SHARED Image.cpp Mosaify.cpp Color.cpp)
#target_include_directories(Mosaify
#        PUBLIC
#        ${CMAKE_SOURCE_DIR}           # Assuming Mosaify.h and Image.h are here
#        ${GLM_INCLUDE_DIRS}
#)
target_include_directories(Mosaify
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/glm>
        $<INSTALL_INTERFACE:include>
)

set(MOSAIFY_INCLUDE ${CMAKE_SOURCE_DIR})

target_link_libraries(
        Mosaify
        PRIVATE
        glm::glm
        nlohmann_json::nlohmann_json
        # ImageMagick::Magick++
        ${ImageMagick_LIBRARIES}
)
set(MOSAIFY_LIBS Mosaify)

# MosaifyPy
#include(FindSWIG)
#find_program(SWIG_PATH swig)
#find_package(SWIG REQUIRED COMPONENTS python)
#include(UseSWIG)
## https://cmake.org/cmake/help/v3.28/module/FindPython3.html#module:FindPython3
#find_package (Python3 COMPONENTS Interpreter Development REQUIRED)
#set_source_files_properties(MosaifyPy.i PROPERTIES CPLUSPLUS ON)
#set_property(SOURCE MosaifyPy.i PROPERTY SWIG_MODULE_NAME MosaifyPy_${CMAKE_SYSTEM_NAME})
#set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
#swig_add_library(MosaifyPy_${CMAKE_SYSTEM_NAME}
#        TYPE MODULE
#        LANGUAGE python
#        SOURCES MosaifyPy.i
#)
#target_include_directories(MosaifyPy_${CMAKE_SYSTEM_NAME}
#        PRIVATE ${Python3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/libboard/include)
#target_link_libraries(
#        MosaifyPy_${CMAKE_SYSTEM_NAME}
#        Mosaify
#        glm::glm
#        board
#        ${Python3_LIBRARIES}
#)


FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0  # Replace with the version you want
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
        hello_test
        hello_test.cpp
)
target_link_libraries(
        hello_test
        GTest::gtest_main
        Mosaify
        glm::glm
)

include(GoogleTest)
gtest_discover_tests(hello_test)

install(TARGETS Mosaify hello_test
        ARCHIVE DESTINATION Mosaify/lib
        LIBRARY DESTINATION Mosaify/lib
        RUNTIME DESTINATION Mosaify/bin
)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MosaifyPy_${CMAKE_SYSTEM_NAME}.py
#        DESTINATION ${CMAKE_SOURCE_DIR}/example)

#set(CMAKE_INSTALL_PYTHON_LIBDIR "${CMAKE_SOURCE_DIR}/example")
#install(TARGETS MosaifyPy_${CMAKE_SYSTEM_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/example)

#FILE(WRITE ${CMAKE_BINARY_DIR}/Mosaify/example/main.py
#        "# Python code example\n"
#        "import MosaifyPy\n"
#        "t = MosaifyPy.Mosaify()\n"
#        "t.setTileSize(8)\n"
#)

install(FILES
        ${CMAKE_SOURCE_DIR}/Mosaify/Mosaify.h
        ${CMAKE_SOURCE_DIR}/Mosaify/Image.h
        DESTINATION Mosaify/include/Mosaify)


