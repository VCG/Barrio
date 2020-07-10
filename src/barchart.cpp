#include "barchart.h"


BarChart::BarChart(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;

  QList<QString> labels;
  labels.append("Syn1");
  labels.append("Syn2");
  labels.append("Syn3");

  QList<float> values;
  values.append(0.7);
  values.append(0.8);
  values.append(0.9);

  data = new BarChartData(labels, values);
}

BarChart::~BarChart()
{
  // destruct
}

QWebEngineView* BarChart::initVisWidget(int ID)
{
  QWebEngineView* m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("barchart_data"), data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

bool BarChart::update()
{
    return false;
}

QWebEngineView* BarChart::getWebEngineView()
{
    return m_web_engine_view;
}

BarChart* BarChart::clone()
{
  return nullptr;
}


BarChartData::BarChartData(QList<QString> labels, QList<float> values) 
{
  m_values = values;
  m_labels = labels;
}

BarChartData::~BarChartData() 
{
  //do nothing for now
}


Q_INVOKABLE QList<float> BarChartData::getData()
{
  return Q_INVOKABLE m_values;
}

Q_INVOKABLE QList<QString> BarChartData::getLabels()
{
  return Q_INVOKABLE m_labels;
}