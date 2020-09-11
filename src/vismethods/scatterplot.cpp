#include "scatterplot.h"

ScatterplotData::ScatterplotData(int ID, GlobalVisParameters* m_global_vis_parameters, DataContainer* data_container)
{

}

ScatterplotData::~ScatterplotData()
{
}

Scatterplot::Scatterplot(Scatterplot* scatterplot)
{
  m_datacontainer = scatterplot->m_datacontainer;
  m_global_vis_parameters = scatterplot->m_global_vis_parameters;
}

Scatterplot::Scatterplot(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Scatterplot::~Scatterplot()
{
}

QWebEngineView* Scatterplot::initVisWidget(int ID, SpecificVisParameters params)
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

QWebEngineView* Scatterplot::getWebEngineView()
{
  return m_web_engine_view;
}

bool Scatterplot::update()
{
  return false;
}

Scatterplot* Scatterplot::clone()
{
  return new Scatterplot(this);
}

VisType Scatterplot::getType()
{
    return VisType::SCATTERPLOT;
}

void Scatterplot::setSpecificVisParameters(SpecificVisParameters params)
{
}

