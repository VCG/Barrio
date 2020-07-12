#include "barchart.h"


BarChart::BarChart(BarChart* barchart)
{
  m_datacontainer = barchart->m_datacontainer;
  m_global_vis_parameters = barchart->m_global_vis_parameters;
}

BarChart::BarChart(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

BarChart::~BarChart()
{
  // destruct
}

QWebEngineView* BarChart::initVisWidget(int ID)
{
  QString json = createJSONString(&m_global_vis_parameters->selectedObjects, m_global_vis_parameters->distance_threshold);
  data = new BarChartData(json);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("barchart_data"), data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

bool BarChart::update()
{
  QString json = createJSONString(&m_global_vis_parameters->selectedObjects, m_global_vis_parameters->distance_threshold);
  data->setJsonString(json);
  return true;
}

QWebEngineView* BarChart::getWebEngineView()
{
    return m_web_engine_view;
}

BarChart* BarChart::clone()
{
  return new BarChart(this);
}

QString BarChart::createJSONString(QList<int>* selected_mitos, double distance_threshold)
{
  QJsonArray json;
  std::map<int, Object*>* objects = m_datacontainer->getObjectsMapPtr();
  std::vector<Object*> synapses = m_datacontainer->getObjectsByType(Object_t::SYNAPSE);

  for each  (int id in *selected_mitos)
  {
    //QJsonObject mito_object;
    //QJsonArray syn_array;

    Object* mito = objects->at(id);
    std::map<int, double>* distance_map = mito->get_distance_map_ptr();

    for each (Object* syn in synapses)
    {
      QString syn_name = syn->getName().c_str();
      double distance_to_mito = distance_map->at(syn->getHVGXID());
      if (distance_to_mito < distance_threshold)
      {
        QJsonObject synapse_object;
        synapse_object.insert("district", QJsonValue::fromVariant(mito->getName().c_str()));
        synapse_object.insert("candidate", QJsonValue::fromVariant(syn_name));
        synapse_object.insert("votes", QJsonValue::fromVariant(distance_to_mito));
        json.push_back(synapse_object);
      }
    }

    //mito_object.insert("categorie", QJsonValue::fromVariant(mito->getName().c_str()));
    //mito_object.insert("values", QJsonValue::fromVariant(syn_array));

    //json.push_back(mito_object);
  }

  QJsonDocument doc(json);
  qDebug() << doc.toJson();

  return doc.toJson(QJsonDocument::Compact);
}


BarChartData::BarChartData(QString json_string) 
{
  m_json_string = json_string;
}

BarChartData::~BarChartData() 
{
  //do nothing for now
}


Q_INVOKABLE QString BarChartData::getData()
{
  return Q_INVOKABLE m_json_string;
}