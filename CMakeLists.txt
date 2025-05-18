cmake_minimum_required(VERSION 3.16)

project(NoteMama VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Gui ShaderTools)
find_package(ZLIB REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appNoteMama
    main.cpp
)

target_include_directories(appNoteMama PRIVATE scene)
target_include_directories(appNoteMama PRIVATE scene/canvas)
target_include_directories(appNoteMama PRIVATE scene/canvas/tools)
target_include_directories(appNoteMama PRIVATE scene/parsers)
target_include_directories(appNoteMama PRIVATE scene/canvas/background)
target_include_directories(appNoteMama PRIVATE scene/remote)
qt_add_qml_module(appNoteMama
    URI NoteMama
    VERSION 1.0
    QML_FILES Main.qml
    QML_FILES ToolSettingsPopup.qml
    SOURCES scene/scene.h scene/scene.cpp
    SOURCES scene/items/sceneitem.h scene/items/sceneitem.cpp
    SOURCES scene/items/vectorpathsceneitem.h scene/items/vectorpathsceneitem.cpp
    SOURCES scene/items/spinnersceneitem.h scene/items/spinnersceneitem.cpp
    SOURCES scene/items/pointsceneitem.h scene/items/pointsceneitem.cpp
    SOURCES scene/canvas/canvas.h scene/canvas/canvas.cpp
    SOURCES scene/canvas/canvasrenderer.h scene/canvas/canvasrenderer.cpp
    SOURCES scene/canvas/tools/tool.h scene/canvas/tools/tool.cpp
    SOURCES scene/canvas/tools/pentool.h scene/canvas/tools/pentool.cpp
    SOURCES scene/canvas/tools/curvepentool.h scene/canvas/tools/curvepentool.cpp
    SOURCES scene/canvas/tools/fakedrawtool.h scene/canvas/tools/fakedrawtool.cpp
    SOURCES scene/parsers/pdfparserdeclarative.h scene/parsers/pdfparserdeclarative.cpp
    SOURCES scene/parsers/pdfparser2/src/pdfparser.h scene/parsers/pdfparser2/src/pdfparser.cpp
    SOURCES scene/parsers/pdfparser2/src/custombuffer/custombuffer.hpp scene/parsers/pdfparser2/src/custombuffer/custombuffer.cpp
    SOURCES scene/parsers/pdfparser2/src/customfunctions/customfunction.h SOURCES scene/parsers/pdfparser2/src/customfunctions/customfunction.cpp
    SOURCES scene/nddgenerator/nddgenerator.h scene/nddgenerator/nddgenerator.cpp
    SOURCES scene/canvas/background/background.h scene/canvas/background/background.cpp
    SOURCES scene/remote/remoteserver.h scene/remote/remoteserver.cpp
    SOURCES scene/remote/remoteclient.h scene/remote/remoteclient.cpp
    SOURCES scene/canvas/tools/selecttool.h scene/canvas/tools/selecttool.cpp
)

qt_add_shaders(appNoteMama "appNoteMama_shaders"
    PRECOMPILE
    OPTIMIZED
    RESOURCE_PREFIX
        /
    FILES
        shaders/color.vert
        shaders/color.frag
        shaders/background.vert
        shaders/background.frag
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
    ZLIB::ZLIB
)

include(GNUInstallDirs)
install(TARGETS appNoteMama
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
