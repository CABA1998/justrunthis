add_library(runners STATIC
    CppRunner.cpp
    CppRunner.h
)

target_include_directories(runners PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(runners PUBLIC Qt6::Core)
