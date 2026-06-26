cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(calculator_app VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CALCULATOR_APP calculator)

FetchContent_Declare(
    calculator_math
    GIT_REPOSITORY git@github.com:GhostRJY/calculator-math.git
    GIT_TAG main
)

FetchContent_MakeAvailable(calculator_math)

add_executable(${CALCULATOR_APP}
    src/main.cpp
)

target_link_libraries(${CALCULATOR_APP}
    PRIVATE
        calculator_math::calculator_math
)

target_compile_options(${CALCULATOR_APP}
    PRIVATE
        -g
        -Wall
        -Wextra
        -Werror
        -Wpedantic
)

set_target_properties(${CALCULATOR_APP}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

find_program(CLANG_FORMAT_PATH NAMES clang-format-21 REQUIRED)

file(
    GLOB_RECURSE FORMAT_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_PATH} -i ${FORMAT_FILES}
    COMMENT "Running clang-format"
)

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_PATH NAMES clang-tidy-21 REQUIRED)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_PATH}")

    set(
        CLANG_TIDY_COMMAND
        ${CLANG_TIDY_PATH}
        "-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )

    set_target_properties(
        ${CALCULATOR_APP}
        PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
    )
endif()

install(
    TARGETS ${CALCULATOR_APP}
    RUNTIME DESTINATION bin
)