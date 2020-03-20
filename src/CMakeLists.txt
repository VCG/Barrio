# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

project( NeCo )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_minimum_required(VERSION 2.8.11)

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

find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL Xml)

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
    glycogen.h
    octree.h
    glycogencluster.h
    glycogenanalysismanager.h
    dbscan.h
    grid3d.h
    spatialhash3d.h
    rendervertexdata.h
    path.h
    performancerate.h
    inputform.h
    mesh_preprocessing.h)

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
    glycogen.cpp
    octree.cpp
    glycogencluster.cpp
    glycogenanalysismanager.cpp
    dbscan.cpp
    grid3d.cpp
    spatialhash3d.cpp
    rendervertexdata.cpp
    path.cpp
    performancerate.cpp
    inputform.cpp
    mesh_preprocessing.cpp)

# Creating entries for target: NeCo

add_executable( NeCo  
    ${project_ui} 
    ${project_headers} 
    ${project_sources} 
    resources.qrc)

add_to_cached_list( CGAL_EXECUTABLE_TARGETS NeCo )

# Link the executable to CGAL and third-party libraries
target_link_libraries( NeCo   ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )

target_link_libraries( NeCo 
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Xml)