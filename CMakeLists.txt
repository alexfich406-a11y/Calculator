cmake_minimum_required(VERSION 3.15)
project(CalculatorProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(plugin_api INTERFACE)
target_include_directories(plugin_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin_api)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(calc main.cpp ${SRC_FILES})
target_include_directories(calc PRIVATE src)
target_link_libraries(calc PRIVATE plugin_api ${CMAKE_DL_LIBS})

function(add_calc_plugin NAME)
    add_library(${NAME} MODULE plugins_src/${NAME}.cpp)
    target_link_libraries(${NAME} PRIVATE plugin_api)
    set_target_properties(${NAME} PROPERTIES
        PREFIX ""                                                 
        LIBRARY_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:calc>/plugins"  
        RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:calc>/plugins" 
    )
    if(APPLE)
        set_target_properties(${NAME} PROPERTIES SUFFIX ".dylib")
    endif()
endfunction()

add_calc_plugin(abs_plugin)
add_calc_plugin(sin_plugin)
add_calc_plugin(cos_plugin)
add_calc_plugin(ln_plugin)
add_calc_plugin(pow_plugin)
add_calc_plugin(max_plugin)


