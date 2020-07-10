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

QWebEngineView* DistanceMatrix::initVisWidget(int ID)
{
  QWebEngineView* m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QString("data"), this);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

bool DistanceMatrix::update()
{
  return false;
}

QWebEngineView* DistanceMatrix::getWebEngineView()
{
  return m_web_engine_view;
}

DistanceMatrix* DistanceMatrix::clone()
{
  return nullptr;
}
