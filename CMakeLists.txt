cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(fashion_mnist VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include(${PROJECT_SOURCE_DIR}/cmake/conan.cmake)
conan_cmake_run(
  CONANFILE conanfile.txt
  BASIC_SETUP
  CMAKE_TARGETS # individual targets to link to
  BUILD
    missing 
)

set(CMAKE_CXX_STANDARD 17)

file(GLOB HRDS
    include/fashion_mnist/*.h
)
file(GLOB SRCS
    src/*.cpp
)

add_library(${PROJECT_NAME}
    ${SRCS}
    ${HRDS}
)

target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/contrib/catboost
)

target_link_libraries(${PROJECT_NAME}
    catboostmodel
    dl
    pthread
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

set(DEMO_COMMON_HDRS
    demo/helpers.h
)

set(DEMO_COMMON_SRCS
    demo/helpers.cpp
)

add_executable(demo_logreg
    demo/demo_logreg.cpp
    ${DEMO_COMMON_SRCS}
    ${DEMO_COMMON_HDRS}
)

target_include_directories(demo_logreg PUBLIC
    include
)

target_link_libraries(demo_logreg
    ${PROJECT_NAME}
)


file(GLOB TEST_SRCS
    tests/*.cpp
)

add_executable(test_${PROJECT_NAME}
    ${TEST_SRCS}
    ${DEMO_COMMON_SRCS}
    ${DEMO_COMMON_HDRS}
)

target_include_directories(test_${PROJECT_NAME} PUBLIC
    include
    demo
)


target_link_libraries(test_${PROJECT_NAME}
    CONAN_PKG::gtest
    ${PROJECT_NAME}
)

add_custom_target(${PROJECT_NAME}_test
    $<TARGET_FILE:test_${PROJECT_NAME}>
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)