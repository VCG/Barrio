# This is the CMake script for compiling NeCo.

project(NeCo)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_minimum_required(VERSION 2.8.11)

if(NOT DEFINED ENV{CEREAL_DIR})
  message(STATUS "This project requires the CEREAL library, and will not be compiled.")
  return()

endif()

include_directories($ENV{CEREAL_DIR})

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()

# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL Xml WebEngine WebEngineCore WebEngineWidgets WebView WebChannel)

set(CMAKE_CXX_STANDARD 17)

set(project_ui
  inputform.ui
  mainwindow.ui)

set(project_headers
    mainwindow.h
    glwidget.h
    mainopengl.h
    object.h
    node.h
    mousepad.h
    edge.h
    graph.h
    colors.h
    abstractionspace.h
    graphmanager.h
    spatialhash.h
    skeleton.h
    skeletonbranch.h
    mesh.h
    ssbo_structs.h
    glsluniform_structs.h
    openglmanager.h
    datacontainer.h
    grid3d.h
    spatialhash3d.h
    rendervertexdata.h
    path.h
    performancerate.h
    inputform.h
    mesh_preprocessing.h
    globalParameters.h
    mainWidget.h
    mainwidget.h
    treemodel.h
    treeitem.h

    vismethods/vismethod.h
    vismethods/barchart.h
    vismethods/scatterplot.h
    vismethods/distancetree.h
    vismethods/distancematrix.h
    vismethods/histogram.h
    vismethods/boxplot.h)

set(project_sources
    main.cpp
    mainwindow.cpp
    mousepad.cpp
    glwidget.cpp
    mainopengl.cpp
    object.cpp
    node.cpp
    edge.cpp
    graph.cpp
    abstractionspace.cpp
    graphmanager.cpp
    spatialhash.cpp
    skeleton.cpp
    skeletonbranch.cpp
    mesh.cpp
    openglmanager.cpp
    datacontainer.cpp
    grid3d.cpp
    spatialhash3d.cpp
    rendervertexdata.cpp
    path.cpp
    performancerate.cpp
    inputform.cpp
    mesh_preprocessing.cpp
    mainwidget.cpp
    treemodel.cpp
    treeitem.cpp

    vismethods/barchart.cpp
    vismethods/scatterplot.cpp
    vismethods/distancetree.cpp
    vismethods/distancematrix.cpp
    vismethods/histogram.cpp
    vismethods/boxplot.cpp

    shaders/coord_system_vert.glsl
    shaders/coord_system_geom.glsl
    shaders/coord_system_frag.glsl
    shaders/selection_frag.glsl
    shaders/selection_geom.glsl
    shaders/selection_vert.glsl
    shaders/hvgx_selection_frag.glsl
    shaders/mesh_vert_simplified.glsl
    shaders/mesh_frag_simplified.glsl)

# Creating entries for target: NeCo

add_executable(NeCo  
    ${project_ui} 
    ${project_headers} 
    ${project_sources} 
    resources.qrc)

add_to_cached_list(CGAL_EXECUTABLE_TARGETS NeCo)

# Link the executable to CGAL and third-party libraries
target_link_libraries(NeCo ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

target_link_libraries(NeCo 
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Xml
    Qt5::WebEngine
    Qt5::WebEngineCore
    Qt5::WebEngineWidgets
    Qt5::WebView
    Qt5::WebChannel)