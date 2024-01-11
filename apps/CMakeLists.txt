project(planarviz-apps)

include_directories(${CMAKE_SOURCE_DIR}/libs/planarviz/include)

add_executable(planarviz-demo planarviz-demo/main.cpp)
target_link_libraries(planarviz-demo PRIVATE planarviz)