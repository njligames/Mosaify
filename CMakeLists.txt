cmake_minimum_required(VERSION 3.27.7)
project(Mosaify)
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "My Comment" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "/System/Volumes/Data/usr/local/Cellar/cmake/3.27.7/share/cmake/Modules/")

# Mosaify target
add_subdirectory(glm)
add_subdirectory(json)
# find_package(ImageMagick COMPONENTS Magick++)
add_library(Mosaify STATIC Image.cpp Mosaify.cpp Color.cpp)
target_include_directories(Mosaify
        INTERFACE public
#        PUBLIC details_public
        PRIVATE ${GLM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(
        Mosaify
        PRIVATE
        glm::glm
        nlohmann_json::nlohmann_json
        # ImageMagick::Magick++
)

# MosaifyPy
include(FindSWIG)
find_program(SWIG_PATH swig)
find_package(SWIG 4.2.1 COMPONENTS python)
include(UseSWIG)
# https://cmake.org/cmake/help/v3.28/module/FindPython3.html#module:FindPython3
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)
set_source_files_properties(MosaifyPy.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE MosaifyPy.i PROPERTY SWIG_MODULE_NAME MosaifyPy)
set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
swig_add_library(MosaifyPy
        TYPE MODULE
        LANGUAGE python
        SOURCES MosaifyPy.i
)
target_include_directories(MosaifyPy
        PRIVATE ${Python3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
target_link_libraries(
        MosaifyPy
        Mosaify
        ${Python3_LIBRARIES}
)

# Testing Mosaify target
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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
)

include(GoogleTest)
gtest_discover_tests(hello_test)

install(TARGETS Mosaify hello_test
        ARCHIVE DESTINATION Mosaify/lib
        LIBRARY DESTINATION Mosaify/lib
        RUNTIME DESTINATION Mosaify/bin
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MosaifyPy.py
        DESTINATION ${CMAKE_SOURCE_DIR}/example)

#set(CMAKE_INSTALL_PYTHON_LIBDIR "${CMAKE_SOURCE_DIR}/example")
install(TARGETS MosaifyPy DESTINATION ${CMAKE_SOURCE_DIR}/example)

FILE(WRITE ${CMAKE_BINARY_DIR}/Mosaify/example/main.py
        "# Python code example\n"
        "import MosaifyPy\n"
        "t = MosaifyPy.Mosaify()\n"
        "t.setTileSize(8)\n"
)

