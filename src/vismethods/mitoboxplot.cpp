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

bool MitoBoxPlot::update_needed()
{
    return true;
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

Q_INVOKABLE void MitoBoxPlotData::setHighlightedFrame(const QString& name)
{
  
  int hvgx = m_datacontainer->getIndexByName(name);
  qDebug() << hvgx;
  if (!m_global_vis_parameters->highlighted_group_boxes.contains(hvgx))
  {
    m_global_vis_parameters->highlighted_group_boxes.append(hvgx);
  }
  return Q_INVOKABLE void();
}

Q_INVOKABLE void MitoBoxPlotData::removeHighlightedFrame(const QString& name_to_remove)
{
  int hvgx_id = m_datacontainer->getIndexByName(name_to_remove);
  QVector<int>* highlighted = &m_global_vis_parameters->highlighted_group_boxes;
  if (highlighted->contains(hvgx_id))
  {
    QMutableVectorIterator<int> it(*highlighted);
    while (it.hasNext())
    {
      if (it.next() == hvgx_id) {
        it.remove();
      }
    }
  }
  return Q_INVOKABLE void();
}

Q_INVOKABLE void MitoBoxPlotData::setHighlightedStructure(const int parentID, int spineNumber)
{
  Object* parent = m_datacontainer->getObjectsMapPtr()->at(parentID);
  std::vector<int>* children_ids = parent->getChildrenIDs();
  for (int i = 0; i < children_ids->size(); i++)
  {
    int id = children_ids->at(i);
    Object* child = m_datacontainer->getObjectsMapPtr()->at(id);
    QString name = child->getName().c_str();
    int my_spine_number = name.split("_")[2].toInt(); // spine number is contained on the 2 position in the array

    if (child->getObjectType() == Object_t::SPINE && my_spine_number == spineNumber && !m_global_vis_parameters->highlighted_objects.contains(id))
    {
      m_global_vis_parameters->highlighted_objects.append(id);
    }
  }
}

Q_INVOKABLE void MitoBoxPlotData::removeHighlightedStructure(const int parentID, int spineNumber)
{
  int hvgx_id = 0;

  Object* parent = m_datacontainer->getObjectsMapPtr()->at(parentID);
  std::vector<int>* children_ids = parent->getChildrenIDs();
  for (int i = 0; i < children_ids->size(); i++)
  {
    int id = children_ids->at(i);
    Object* child = m_datacontainer->getObjectsMapPtr()->at(id);
    QString name = QString::fromUtf8(child->getName().c_str());
    int my_spine_number = name.split("_")[2].toInt(); // spine number is contained on the 2 position in the array

    if (my_spine_number == spineNumber)
    {
      hvgx_id = id;
    }
  }

  QVector<int>* highlighted = &m_global_vis_parameters->highlighted_objects;
  if (highlighted->contains(hvgx_id))
  {
    QMutableVectorIterator<int> it(*highlighted);
    while (it.hasNext())
    {
      if (it.next() == hvgx_id) {
        it.remove();
      }
    }
  }
}
