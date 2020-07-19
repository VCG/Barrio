#include "distancematrix.h"

DistanceMatrix::DistanceMatrix(DistanceMatrix* matrix)
{
  m_datacontainer = matrix->m_datacontainer;
}

DistanceMatrix::DistanceMatrix(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
}

DistanceMatrix::~DistanceMatrix()
{
  // do nothing
}

QWebEngineView* DistanceMatrix::initVisWidget(int ID)
{
  QList<int> list({ 1064, 1073 });
  QString json = getJSONString(&list);
  data = new DistanceMatrixData(json);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("distancematrix_data"), data);
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
  return new DistanceMatrix(this);
}

QString DistanceMatrix::getJSONString(QList<int>* selected_mitos)
{
  QJsonArray json;
  std::map<int, Object*>* objects = m_datacontainer->getObjectsMapPtr();
  std::vector<Object*> synapses = m_datacontainer->getObjectsByType(Object_t::SYNAPSE);

  for each (int id in *selected_mitos)
  {
    QJsonObject mito_object;
    Object* mito = objects->at(id);
    std::map<int, double>* distance_map = mito->get_distance_map_ptr();
    mito_object.insert("name", QJsonValue::fromVariant(mito->getName().c_str()));
    
    QJsonArray syn_array;
    for each (Object* syn in synapses)
    {
      int id = syn->getHVGXID();
      double distance_to_mito = distance_map->at(id);

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

DistanceMatrixData::DistanceMatrixData(QString json_string)
{
  m_json_string = json_string;
}

DistanceMatrixData::~DistanceMatrixData()
{
  // do nothing for now
}

Q_INVOKABLE QString DistanceMatrixData::getData()
{
  return Q_INVOKABLE m_json_string;
}
