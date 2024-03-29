project(planarviz VERSION 0.2)
set(namespace planarviz)

find_package(fmt REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(CGAL REQUIRED)
find_package(GMP REQUIRED)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()

include_directories(${CMAKE_SOURCE_DIR}/third-party/stb)
include_directories(${CMAKE_SOURCE_DIR}/third-party/imgui)
include_directories(${CMAKE_SOURCE_DIR}/third-party/json/include)

#////////////////////////////////////////////////////
#/ Setup shaders

include(SetupShaders)
setup_shaders("${PROJECT_SOURCE_DIR}/include/planarviz_bundled_shaders.h")

#////////////////////////////////////////////////////

add_library(${PROJECT_NAME}  SHARED)

set(public_headers 
    include/planarviz.h
    include/_ImGui.h
    include/Camera.h
    include/Color.h
    include/Geometry.h
    include/Scene.h
    include/Shader.h
    include/Texture.h
    include/Utils.h
    include/PlanarVizApplication.h
    include/Constants.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_sources(${PROJECT_NAME} PRIVATE
    src/Utils.cpp
    src/Scene.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Geometry.cpp
    src/PlanarVizApplication.cpp

    ${CMAKE_SOURCE_DIR}/third-party/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/misc/cpp/imgui_stdlib.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/backends/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/third-party/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    # ${PROJECT_SOURCE_DIR}/include
    ${fmt_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt SDL2::SDL2 GLEW::GLEW ${GMP_LIBRARIES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
    DESTINATION cmake
)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)
# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)
# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)
