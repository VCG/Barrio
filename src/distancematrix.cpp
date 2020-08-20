#include "distancematrix.h"

DistanceMatrix::DistanceMatrix(DistanceMatrix* matrix)
{
  m_datacontainer = matrix->m_datacontainer;
  m_global_vis_parameters = matrix->m_global_vis_parameters;
}

DistanceMatrix::DistanceMatrix(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
  m_global_vis_parameters = visparams;
}

DistanceMatrix::~DistanceMatrix()
{
  // do nothing
}

QWebEngineView* DistanceMatrix::initVisWidget(int ID)
{
  QString json = getJSONString(&m_global_vis_parameters->selected_objects, m_global_vis_parameters->distance_threshold);
  data = new DistanceMatrixData(json, m_datacontainer, m_global_vis_parameters);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("distancematrix_data"), data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

bool DistanceMatrix::update()
{
  QString json = getJSONString(&m_global_vis_parameters->selected_objects, m_global_vis_parameters->distance_threshold);
  data->setJsonString(json);
  return true;
}

QWebEngineView* DistanceMatrix::getWebEngineView()
{
  return m_web_engine_view;
}

DistanceMatrix* DistanceMatrix::clone()
{
  return new DistanceMatrix(this);
}

QString DistanceMatrix::getJSONString(QList<int>* selected_mitos, double distanceThreshold)
{
  QJsonArray json;
  std::map<int, Object*>* objects = m_datacontainer->getObjectsMapPtr();
  std::vector<Object*> synapses = m_datacontainer->getObjectsByType(Object_t::SYNAPSE);

  QList<int> selected_synapses;
  for each (int mito_id in *selected_mitos)
  {
    Object* mito = objects->at(mito_id);
    std::map<int, double>* distance_map = mito->get_distance_map_ptr();

    for each(Object* syn in synapses)
    {
      double distance_to_mito = distance_map->at(syn->getHVGXID());
      if (distance_to_mito < distanceThreshold && !selected_synapses.contains(syn->getHVGXID()))
      {
        selected_synapses.append(syn->getHVGXID());
      }
    }
  }

  for each (int mito_id in *selected_mitos)
  {
    QJsonObject mito_object;
    Object* mito = objects->at(mito_id);
    std::map<int, double>* distance_map = mito->get_distance_map_ptr();
    mito_object.insert("name", QJsonValue::fromVariant(mito->getName().c_str()));
    
    QJsonArray syn_array;
    for each (int syn_id in selected_synapses)
    {
      Object* syn = objects->at(syn_id);
      double distance_to_mito = distance_map->at(syn_id);

      QJsonObject syn_object;
      syn_object.insert("name", QJsonValue::fromVariant(syn->getName().c_str()));
      syn_object.insert("distance", QJsonValue::fromVariant(distance_to_mito));
      syn_array.push_back(syn_object);

    }
    mito_object.insert("synapses", syn_array);
    json.push_back(mito_object);
  }
  QJsonDocument doc(json);
  return doc.toJson(QJsonDocument::Compact);
}

DistanceMatrixData::DistanceMatrixData(QString json_string, DataContainer* datacontainer, GlobalVisParameters* visparameters)
{
  m_json_string = json_string;

  m_datacontainer = datacontainer;
  m_global_vis_parameters = visparameters;
}

DistanceMatrixData::~DistanceMatrixData()
{
  // do nothing for now
}

Q_INVOKABLE QString DistanceMatrixData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE void DistanceMatrixData::setHighlightedStructure(const QString& name)
{
  int hvgx_id = m_datacontainer->getIndexByName(name);
  if (!m_global_vis_parameters->highlighted_objects.contains(hvgx_id))
  {
    m_global_vis_parameters->highlighted_objects.append(hvgx_id);
  }
}

Q_INVOKABLE void DistanceMatrixData::removeHighlightedStructure(const QString& name_to_remove)
{
  int hvgx_id = m_datacontainer->getIndexByName(name_to_remove);
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
