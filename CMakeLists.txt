cmake_minimum_required(VERSION 3.28)
project(Mosaify)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Mosaify target
add_subdirectory(glm)
add_library(Mosaify STATIC Mosaify.cpp Image.cpp)
target_include_directories(Mosaify
        INTERFACE public
#        PUBLIC details_public
        PRIVATE ${GLM_INCLUDE_DIRS})
target_link_libraries(
        Mosaify
        glm::glm
)

# MosaifyPy
include(FindSWIG)

find_program(SWIG_PATH swig)
find_package(SWIG 4.0 COMPONENTS python)

include(UseSWIG)

set(CMAKE_MODULE_PATH "/System/Volumes/Data/usr/local/Cellar/cmake/3.27.7/share/cmake/Modules/")
message(STATUS "This is the path ${CMAKE_MODULE_PATH}")
#find_package(PythonLibs REQUIRED)
#find_package(PythonInterp ${PYTHONLIBS_VERSION_STRING} REQUIRED)
set(CMAKE_SWIG_FLAGS -py3)
find_package (Python3 COMPONENTS Interpreter Development)

message(STATUS "dirs ${Python3_INCLUDE_DIRS}")






set_source_files_properties(MosaifyPy.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE MosaifyPy.i PROPERTY SWIG_MODULE_NAME MosaifyPy)
set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)

swig_add_library(MosaifyPy
        TYPE SHARED
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

