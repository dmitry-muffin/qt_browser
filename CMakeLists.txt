cmake_minimum_required(VERSION 3.31)
project(d_browser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        WebEngineWidgets
        Network
        REQUIRED)

add_executable(untitled main.cpp
        browser.cpp
        browser.h
        answerwindow.cpp
        answerwindow.h
)
target_link_libraries(untitled
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::WebEngineWidgets
        Qt::Network
)

