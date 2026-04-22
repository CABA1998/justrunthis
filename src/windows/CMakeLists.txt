add_library(windows STATIC
    MainWindow.cpp
    MainWindow.h
)

target_include_directories(windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(windows
    PUBLIC Qt6::Widgets
    PRIVATE runners
)
