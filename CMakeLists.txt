cmake_minimum_required(VERSION 3.5)

project (flat-sdf)

set(SOURCES
    src/fsdf_image.cpp
    src/fsdf_renderer.cpp
    src/fsdf_shapes.cpp
    src/main.cpp
)

add_executable (flat-sdf ${SOURCES})

target_include_directories (flat-sdf
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)