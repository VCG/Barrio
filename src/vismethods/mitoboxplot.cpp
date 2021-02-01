#include "mitoboxplot.h"

MitoBoxPlot::MitoBoxPlot(MitoBoxPlot* mitoScheme)
{
  m_datacontainer = mitoScheme->m_datacontainer;
  m_global_vis_parameters = mitoScheme->m_global_vis_parameters;
}

MitoBoxPlot::MitoBoxPlot(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

MitoBoxPlot::~MitoBoxPlot()
{
}

QString MitoBoxPlot::createJSONString(QList<int>* selectedObjects)
{
    return QString();
}

QWebEngineView* MitoBoxPlot::initVisWidget(int ID, SpecificVisParameters params)
{
  m_data = new MitoBoxPlotData(ID, m_datacontainer, m_global_vis_parameters);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("mito_scheme_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* MitoBoxPlot::getWebEngineView()
{
    return nullptr;
}

bool MitoBoxPlot::update()
{
    return false;
}

MitoBoxPlot* MitoBoxPlot::clone()
{
    return new MitoBoxPlot(this);
}

VisType MitoBoxPlot::getType()
{
    return VisType::MITO_BOX_PLOT;
}

void MitoBoxPlot::setSpecificVisParameters(SpecificVisParameters params)
{
}

MitoBoxPlotData::MitoBoxPlotData(int ID, DataContainer* data_container, GlobalVisParameters* global_vis_parameters)
{
  //TODO
}

MitoBoxPlotData::~MitoBoxPlotData()
{
}

Q_INVOKABLE QString MitoBoxPlotData::getData()
{
  return Q_INVOKABLE QString();
}

Q_INVOKABLE int MitoBoxPlotData::getID()
{
    return Q_INVOKABLE int();
}

Q_INVOKABLE void MitoBoxPlotData::setHighlightedStructure(const int parentID, int spineNumber)
{
  return Q_INVOKABLE void();
}

Q_INVOKABLE void MitoBoxPlotData::removeHighlightedStructure(const int parentID, int spineNumber)
{
  return Q_INVOKABLE void();
}
