#include "boxplot.h"

Boxplot::Boxplot(Boxplot* boxplot)
{
  m_datacontainer = boxplot->m_datacontainer;
  m_global_vis_parameters = boxplot->m_global_vis_parameters;
}

Boxplot::Boxplot(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Boxplot::~Boxplot()
{
}

QWebEngineView* Boxplot::initVisWidget(int ID, SpecificVisParameters params)
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

QWebEngineView* Boxplot::getWebEngineView()
{
  return m_web_engine_view;
}

bool Boxplot::update()
{
  return false;
}

Boxplot* Boxplot::clone()
{
  return new Boxplot(this);
}

VisType Boxplot::getType()
{
    return VisType::BOXPLOT;
}

BoxplotData::BoxplotData(int ID, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container)
{
}

BoxplotData::~BoxplotData()
{
}
