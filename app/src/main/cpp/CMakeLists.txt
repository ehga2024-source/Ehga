cmake_minimum_required(VERSION 3.22.1)
project("ehgarranger")

add_library(
        ehgarranger
        SHARED
        native-lib.cpp
        arranger_engine.cpp
)

find_library(log-lib log)

target_link_libraries(
        ehgarranger
        ${log-lib}
)
