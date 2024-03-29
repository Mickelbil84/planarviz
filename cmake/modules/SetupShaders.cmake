function(setup_shaders SHADERS_INCLUDE)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/resources/shaders)

set(DEFAULT_SHADER_NAME "default")

set(DEFAULT_VERTEX_SHADER_EXISTS FALSE)
set(DEFAULT_FRAGMENT_SHADER_EXISTS FALSE)

file(GLOB SHADER_FILES ${SHADERS_DIR}/*.vs ${SHADERS_DIR}/*.fs)
file(REMOVE ${SHADERS_INCLUDE})

file(APPEND ${SHADERS_INCLUDE} "// Auto Generated File: DO NOT EDIT!\n")
file(APPEND ${SHADERS_INCLUDE} "#pragma once\n")
file(APPEND ${SHADERS_INCLUDE} "namespace ${namespace} {\n")
foreach(SHADER_FILE ${SHADER_FILES})
    file(READ ${SHADER_FILE} SHADER_CONTENT)
    # string(REPLACE "\n" "\n\t\t" SHADER_CONTENT ${SHADER_CONTENT})

    get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
    string(REPLACE "." "_" SHADER_FILE_NAME ${SHADER_FILE_NAME})

    if (${SHADER_FILE_NAME} STREQUAL "${DEFAULT_SHADER_NAME}_vs")
        set(DEFAULT_VERTEX_SHADER_EXISTS TRUE)
    endif()
    if (${SHADER_FILE_NAME} STREQUAL "${DEFAULT_SHADER_NAME}_fs")
        set(DEFAULT_FRAGMENT_SHADER_EXISTS TRUE)
    endif()

    file(APPEND ${SHADERS_INCLUDE} "\tconstexpr const char* ${SHADER_FILE_NAME} = R\"V0G0N(${SHADER_CONTENT})V0G0N\";\n\n")
endforeach()

if (NOT ${DEFAULT_VERTEX_SHADER_EXISTS} OR NOT ${DEFAULT_FRAGMENT_SHADER_EXISTS})
    message(ERROR "Default shader does not exist. Expected to find: ${SHADERS_DIR}/${DEFAULT_SHADER_NAME}.[vs|fs]")
endif()

file(APPEND ${SHADERS_INCLUDE} "\n")
file(APPEND ${SHADERS_INCLUDE} "\t#define DEFAULT_VERTEX_SHADER ${DEFAULT_SHADER_NAME}_vs\n")
file(APPEND ${SHADERS_INCLUDE} "\t#define DEFAULT_FRAGMENT_SHADER ${DEFAULT_SHADER_NAME}_fs\n")

file(APPEND ${SHADERS_INCLUDE} "\}")

endfunction()