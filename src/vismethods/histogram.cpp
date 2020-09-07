#include "histogram.h"

Histogram::Histogram(Histogram* histogram)
{
  m_datacontainer = histogram->m_datacontainer;
  m_global_vis_parameters = histogram->m_global_vis_parameters;
}

Histogram::Histogram(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Histogram::~Histogram()
{
}

QWebEngineView* Histogram::initVisWidget(int ID)
{
  //QString json = createJSONString(&m_global_vis_parameters->selected_objects, m_global_vis_parameters->distance_threshold);
  //data = new BarChartData(json, m_datacontainer, m_global_vis_parameters);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  //channel->registerObject(QStringLiteral("barchart_data"), data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* Histogram::getWebEngineView()
{
  return m_web_engine_view;
}

bool Histogram::update()
{
  return false;
}

Histogram* Histogram::clone()
{
  return new Histogram(this);
}

HistogramData::HistogramData(int ID, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container)
{

}

HistogramData::~HistogramData()
{
}
