cmake_minimum_required(VERSION 3.16)

project(NoteMama VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Gui ShaderTools)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appNoteMama
    main.cpp
)

target_include_directories(appNoteMama PRIVATE canvas)
qt_add_qml_module(appNoteMama
    URI NoteMama
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES canvas/canvas.h canvas/canvas.cpp
    SOURCES canvas/canvasrenderer.h canvas/canvasrenderer.cpp
    SOURCES canvas/items/vectorpathcanvasitem.h canvas/items/vectorpathcanvasitem.cpp
    SOURCES canvas/tools/pentool.h canvas/tools/pentool.cpp
    SOURCES canvas/tools/tool.h canvas/tools/tool.cpp
    SOURCES canvas/items/canvasitem.h canvas/items/canvasitem.cpp
    SOURCES canvas/items/spinnercanvasitem.h canvas/items/spinnercanvasitem.cpp
    SOURCES canvas/tools/curvepentool.h canvas/tools/curvepentool.cpp
    SOURCES canvas/tools/fakedrawtool.h canvas/tools/fakedrawtool.cpp
)

qt_add_shaders(appNoteMama "appNoteMama_shaders"
    PRECOMPILE
    OPTIMIZED
    RESOURCE_PREFIX
        /
    FILES
        shaders/color.vert
        shaders/color.frag
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appNoteMama PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appNoteMama
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appNoteMama
    PRIVATE Qt6::Quick Qt6::GuiPrivate
)

include(GNUInstallDirs)
install(TARGETS appNoteMama
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
