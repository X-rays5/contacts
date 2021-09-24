cmake_minimum_required(VERSION 3.16)
project(contacts)

set(CMAKE_CXX_STANDARD 20)

set(SIMPLE_DB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/simple_db)
set(INIREADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/inireader)
set(RAPIDJSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/rapidjson)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SRC
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/*.h"
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/leveldb)

find_package(OpenGL)

add_executable(contacts ${IMGUI_SRC} src/main.cpp src/opengl.hpp src/ui/ui.cpp src/ui/ui.hpp src/tabs/contacts.hpp src/tabs/settings.hpp src/json.hpp)

target_include_directories(contacts PRIVATE ${SIMPLE_DB_DIR})
target_include_directories(contacts PRIVATE ${INIREADER_DIR})
target_include_directories(contacts PRIVATE ${RAPIDJSON_DIR}/include)
target_include_directories(contacts PRIVATE ${IMGUI_DIR})

target_link_libraries(contacts PRIVATE SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(contacts PRIVATE leveldb)
target_link_libraries(contacts PRIVATE opengl32)