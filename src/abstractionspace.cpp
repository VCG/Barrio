#include "abstractionspace.h"
#include <QDebug>

//case 1: pos1 = mesh_vertex; break;
//case 2: pos1 = vec4(Vskeleton_vx.xyz, 1.0); break;
//case 3: pos1 = center4d; break;
//case 4: pos1 = projected_point; break;

/*
 * Todo: add the interpolation states for the abstract graph and the 3D data
 * complete the space
 */
AbstractionSpace::AbstractionSpace()
{
  qDebug() << "Hello abstraction space!";
}

AbstractionSpace::~AbstractionSpace()
{
  qDebug() << "~AbstractionSpace";
}

void AbstractionSpace::configureVisMethods(VisConfiguration config)
{
  qDebug() << "Decide on Vis Methods";
}
