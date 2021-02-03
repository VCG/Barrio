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

QString MitoBoxPlot::createJSONString(DataContainer* data_container, QList<int>* selectedObjects)
{
  QString json_string = data_container->m_sematic_skeleton_json;
  QJsonDocument doc = QJsonDocument::fromJson(json_string.toUtf8());
  std::map<int, Object*>* objects = data_container->getObjectsMapPtr();

  QJsonArray output;

  for (auto it : doc.array()) {
    QJsonObject element = it.toObject();
    int id = element["id"].toInt();

    if (selectedObjects->contains(id))
    {
      output.append(element);
    }

    Object* obj = objects->at(id);
    for (auto selected : *selectedObjects)
    {
      if (obj->isChild(selected))
      {
        output.append(element);
      }
    }
  }

  QJsonDocument out_doc;
  out_doc.setArray(output);

  return QString(out_doc.toJson());
}

QWebEngineView* MitoBoxPlot::initVisWidget(int ID, SpecificVisParameters params)
{
  QString json = createJSONString(m_datacontainer, &m_global_vis_parameters->selected_objects);
  m_data = new MitoBoxPlotData(json, m_datacontainer, m_global_vis_parameters);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("mitoboxplot_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* MitoBoxPlot::getWebEngineView()
{
    return m_web_engine_view;
}

bool MitoBoxPlot::update()
{
  QString json = createJSONString(m_datacontainer, &m_global_vis_parameters->selected_objects);
  m_data->setJSONString(json);
  return true;
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

/*---------------------------------------------------------------------------*/

MitoBoxPlotData::MitoBoxPlotData(QString json, DataContainer* data_container, GlobalVisParameters* global_vis_parameters)
{
  m_json_string = json;
  m_datacontainer = data_container;
  m_global_vis_parameters = global_vis_parameters;
}

MitoBoxPlotData::~MitoBoxPlotData()
{
}

Q_INVOKABLE QString MitoBoxPlotData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE void MitoBoxPlotData::setHighlightedStructure(const int parentID, int spineNumber)
{
  return Q_INVOKABLE void();
}

Q_INVOKABLE void MitoBoxPlotData::removeHighlightedStructure(const int parentID, int spineNumber)
{
  return Q_INVOKABLE void();
}
