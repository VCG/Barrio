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
    Object* mito = objects->at(id);
    std::map<int, double>* distance_map = mito->get_distance_map_ptr();
    std::map<QString, float> selected_syns;

    int group_synapse_counter = 0;
    for each (Object* syn in synapses)
    {
      QString syn_name = syn->getName().c_str();
      double distance_to_mito = distance_map->at(syn->getHVGXID());
      if (distance_to_mito < distance_threshold)
      {
        selected_syns[syn_name] = distance_to_mito;
        group_synapse_counter++;
      }
    }

    // sort map
    std::vector<std::pair<QString, float>> vec;
    std::map<QString, float> ::iterator it2;

    for (it2 = selected_syns.begin(); it2 != selected_syns.end(); it2++)
    {
      vec.push_back(std::make_pair(it2->first, it2->second));
    }
    std::sort(vec.begin(), vec.end(), sortByVal);

    for (int i = 0; i < vec.size(); i++)
    {
      QJsonObject synapse_object;
      synapse_object.insert("district", QJsonValue::fromVariant(mito->getName().c_str()));
      synapse_object.insert("candidate", QJsonValue::fromVariant(vec[i].first));
      synapse_object.insert("votes", QJsonValue::fromVariant(vec[i].second));
      synapse_object.insert("group_size", QJsonValue::fromVariant(group_synapse_counter));
      json.push_back(synapse_object);
    }

  }

  QJsonDocument doc(json);
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