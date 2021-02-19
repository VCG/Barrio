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

QWebEngineView* BarChart::initVisWidget(int ID, SpecificVisParameters params)
{
  m_settings = params.settings;

  QString json = createJSONString(&m_global_vis_parameters->selected_objects, m_global_vis_parameters->distance_threshold);
  m_data = new BarChartData(json, m_datacontainer, m_global_vis_parameters);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("barchart_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

bool BarChart::update()
{
  QString json = createJSONString(&m_global_vis_parameters->selected_objects, m_global_vis_parameters->distance_threshold);
  m_data->setJsonString(json);
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

bool BarChart::update_needed()
{
    return true;
}

VisType BarChart::getType()
{
  return VisType::BARCHART;
}


QString BarChart::createJSONString(QList<int>* selected_mitos, double distance_threshold)
{
  QString related_synapses = "related-synapses";
  QString surrounding_synapses = "surrounding-synapses";
  QString synapse_param = m_settings.value("params").toString();

  QJsonArray json;
  std::map<int, Object*>* objects = m_datacontainer->getObjectsMapPtr();
  std::vector<Object*> synapses = m_datacontainer->getObjectsByType(Object_t::SYNAPSE);

  for each (int id in *selected_mitos)
  {
    Object* mito = objects->at(id);
    int cell_id = mito->getParentID();
    Object* cell = objects->at(cell_id);

    std::map<int, double>* distance_map = mito->get_distance_map_ptr();
    std::map<QString, float> selected_syns;

    int group_synapse_counter = 0;
    if (synapse_param == related_synapses) 
    {
      for each (Object * syn in *cell->getSynapses())
      {
        QString syn_name = syn->getName().c_str();
        double distance_to_mito = distance_map->at(syn->getHVGXID());

        selected_syns[syn_name] = distance_to_mito;
        group_synapse_counter++;

      }
    }
    else if (synapse_param == surrounding_synapses)
    {
      for each (Object * syn in synapses)
      {
        QString syn_name = syn->getName().c_str();
        double distance_to_mito = distance_map->at(syn->getHVGXID());

        if (distance_to_mito < distance_threshold) 
        {
          selected_syns[syn_name] = distance_to_mito;
          group_synapse_counter++;
        }
      }
    }
    

    // sort map
    std::vector<std::pair<QString, float>> vec;
    for (auto it = selected_syns.begin(); it != selected_syns.end(); it++)
    {
      vec.push_back(std::make_pair(it->first, it->second));
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

void BarChart::setSpecificVisParameters(SpecificVisParameters params)
{
}


BarChartData::BarChartData(QString json_string, DataContainer* datacontainer, GlobalVisParameters* visparameters)
{
  m_json_string = json_string;

  m_datacontainer = datacontainer;
  m_global_vis_parameters = visparameters;
}

BarChartData::~BarChartData()
{
  //do nothing for now
}


Q_INVOKABLE QString BarChartData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE void BarChartData::setHighlightedStructure(const QString& name)
{
  int hvgx_id = m_datacontainer->getIndexByName(name);
  if (!m_global_vis_parameters->highlighted_objects.contains(hvgx_id))
  {
    m_global_vis_parameters->highlighted_objects.append(hvgx_id);
  }
}

Q_INVOKABLE void BarChartData::removeHighlightedStructure(const QString& name_to_remove)
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