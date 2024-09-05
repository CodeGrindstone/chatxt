cmake_minimum_required(VERSION 3.5)

project(Chatxt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Network REQUIRED) # 网络库

include_directories(HEADER)

set(PROJECT_SOURCES
        ./Sources/main.cpp
        ./Sources/mainwindow.cpp
        ./Header/mainwindow.h
        ./UI/mainwindow.ui
)

set(SOURCES
    Sources/logindialog.cpp
    Sources/registerdialog.cpp
    Sources/global.cpp
    Sources/HttpMgr.cpp
)

set(HEADERS
    Header/logindialog.h
    Header/registerdialog.h
    Header/global.h
    Header/Singleton.h
    Header/HttpMgr.h
)

set(UIS
    UI/logindialog.ui
    UI/registerdialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Chatxt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${HEADERS}
        ${SOURCES}
        ${UIS}
        rc.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Chatxt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Chatxt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Chatxt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Chatxt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Chatxt PRIVATE Qt6::Network)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Chatxt)
endif()
set_target_properties(Chatxt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Chatxt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Chatxt)
endif()
