cmake_minimum_required(VERSION 3.3)
project(SnakeGame)

# Use C++17
set(CMAKE_CXX_STANDARD 17)

# Adicione os diretórios de include
include_directories(include)

# Adicione as bibliotecas necessárias
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Adicione o diretório src como fonte
file(GLOB SOURCES src/*.cpp)

add_executable(SnakeGame ${SOURCES})

# Linkando as bibliotecas
target_link_libraries(SnakeGame GLAD glfw OpenGL::GL)
