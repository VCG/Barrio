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
    mesh_preprocessing.h
    globalParameters.h
    mainWidget.h
    mainwidget.h
    treemodel.h
    treeitem.h
    vismethod.h
    barchart.h
    scatterplot.h)

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
    mesh_preprocessing.cpp
    mainwidget.cpp
    treemodel.cpp
    treeitem.cpp
    barchart.cpp
    scatterplot.cpp
    shaders/coord_system_vert.glsl
    shaders/coord_system_geom.glsl
    shaders/coord_system_frag.glsl
    shaders/slice_vert.glsl
    shaders/slice_geom.glsl
    shaders/slice_frag.glsl
    shaders/mesh_frag.glsl
    shaders/mesh_geom.glsl
    shaders/mesh_vert.glsl
    shaders/selection_frag.glsl
    shaders/selection_geom.glsl
    shaders/selection_vert.glsl
    shaders/skeleton_point_geom.glsl
    shaders/skeleton_point_vert.glsl
    shaders/space2d_geom.glsl
    shaders/space2d_vert.glsl
    shaders/text_frag.glsl
    shaders/text_geom.glsl
    shaders/text_vert.glsl
    shaders/space_pointer_frag.glsl
    shaders/space_pointer_geom.glsl
    shaders/space_pointer_vert.glsl
    shaders/abstract_skeleton_line_geom.glsl
    shaders/nodes_vert.glsl
    shaders/abstract_skeleton_node_geom.glsl
    shaders/points_3d_frag.glsl
    shaders/points_passthrough_frag.glsl
    shaders/abstract_skeleton_node_2D_vert.glsl
    shaders/abstract_skeleton_node_transition_vert.glsl
    shaders/common.glslh
    shaders/glycogen_frag.glsl
    shaders/glycogen_geom.glsl
    shaders/glycogen_vert.glsl
    shaders/hvgx_selection_frag.glsl
    shaders/nodes_2DHeatMap_frag.glsl
    shaders/nodes_2DHeatMap_geom.glsl
    shaders/nodes_2DHeatMap_vert.glsl
    shaders/nodes_2DHeatMap_tex_frag.glsl
    shaders/nodes_2DHeatMap_tex_geom.glsl
    shaders/nodes_2DHeatMap_tex_vert.glsl
    shaders/space2d_grid_geom.glsl
    shaders/path_frag.glsl
    shaders/path_geom.glsl
    shaders/path_vert.glsl
    shaders/space2d_grid_points_geom.glsl
    shaders/space2d_grid_triangle_geom.glsl
    shaders/skeleton_point_frag.glsl
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