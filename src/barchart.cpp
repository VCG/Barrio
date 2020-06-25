#include "barchart.h"


BarChart::BarChart()
{
  m_values.append(0.7);
  m_values.append(0.8);
  m_values.append(0.9);

  m_labels.append("Syn1");
  m_labels.append("Syn2");
  m_labels.append("Syn3");
}

BarChart::~BarChart()
{
  // destruct
}

Q_INVOKABLE QList<float> BarChart::getData()
{
  return Q_INVOKABLE m_values;
}

Q_INVOKABLE QList<QString> BarChart::getLabels()
{
  return Q_INVOKABLE m_labels;
}

QWebEngineView* BarChart::getVisWidget()
{
  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("data"), this);
  view->load(getHTMLPath(m_index_filename));

  return view;
}