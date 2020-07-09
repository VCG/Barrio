#include "distancematrix.h"

DistanceMatrix::DistanceMatrix(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
}

DistanceMatrix::~DistanceMatrix()
{
}

Q_INVOKABLE QList<float> DistanceMatrix::getData()
{
    return Q_INVOKABLE QList<float>();
}

Q_INVOKABLE QList<QString> DistanceMatrix::getLabels()
{
    return Q_INVOKABLE QList<QString>();
}

QWebEngineView* DistanceMatrix::getVisWidget(int ID)
{
  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("data"), this);
  view->load(getHTMLPath(m_index_filename));

  return view;
}
