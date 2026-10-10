cmake_minimum_required(VERSION 3.16)

project(carrom_vision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(carrom_vision discvision.cpp)

target_include_directories(
    carrom_vision PRIVATE ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    carrom_vision PRIVATE ${OpenCV_LIBS}
)
