cmake_minimum_required(VERSION 3.17)
project(FourierSeriesLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)




find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

add_executable(FourierSeriesLab
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)


target_link_libraries(FourierSeriesLab PRIVATE
        Qt6::Widgets
        Qt6::Charts
)
