cmake_minimum_required(VERSION 3.16)

project(VFW)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

set(CMAKE_UNITY_BUILD ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Multimedia
    MultimediaWidgets
)

include_directories(source/headers)

file(GLOB_RECURSE SRC_FILES
    source/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
    source/headers/*.h
)

add_executable(VFW
    ${SRC_FILES}
    ${HEADER_FILES}
    assets/app_icon.rc
)

target_precompile_headers(VFW PRIVATE
    <QVBoxLayout>
    <QPushButton>
    <QString>
    <QDialog>
    <QLabel>
    <QUrl>
    <QHBoxLayout>
    <QGridLayout>
    <QScrollArea>
    <QMenu>
    <QApplication>
    <QWidget>
    <QToolButton>
    <QPixmap>
    <QLineEdit>
    <QGraphicsView>
    <QGraphicsTextItem>
    <QMainWindow>
    <QMediaPlayer>
    <QAudioOutput>
    <QGraphicsVideoItem>
    <QProcess>
    <QFileDialog>
    <vector>
    <string>
    <filesystem>
)

target_link_libraries(VFW
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

if(WIN32)
    target_compile_definitions(VFW PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_options(VFW PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>
    )
endif()
