#include "distancetree.h"


DistanceTree::DistanceTree(DistanceTree* distanceTree)
{
  m_datacontainer = distanceTree->m_datacontainer;
  m_global_vis_parameters = distanceTree->m_global_vis_parameters;
}

DistanceTree::DistanceTree(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
  m_global_vis_parameters = visparams;
}

DistanceTree::~DistanceTree()
{
  // destruct distance tree
}

QWebEngineView* DistanceTree::initVisWidget(int ID, SpecificVisParameters params)
{
  m_settings = params.settings;

  QString newickString = createNewickString(ID, m_global_vis_parameters->distance_threshold);
  data = new DistanceTreeData(ID, newickString, m_global_vis_parameters, m_datacontainer);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QString("distance_tree_data"), data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* DistanceTree::getWebEngineView()
{
  return m_web_engine_view;
}

bool DistanceTree::update()
{
  int id = data->m_hvgxID;
  QString newick = createNewickString(id, m_global_vis_parameters->distance_threshold);
  data->setNewickString(newick);
  return true;
}

DistanceTree* DistanceTree::clone()
{
  return new DistanceTree(this);
}

bool DistanceTree::update_needed()
{
  return true;
}

VisType DistanceTree::getType()
{
  return VisType::DISTANCE_TREE;
}

void DistanceTree::setSpecificVisParameters(SpecificVisParameters params)
{
}

/*
  hvgxID must be a mitochondrion
*/
QString DistanceTree::createNewickString(int root_hvgx_id, float distanceThreshold)
{
  QString related_synapses = "related-synapses";
  QString surrounding_synapses = "surrounding-synapses";
  QString synapse_param = m_settings.value("params").toString();

  QString newickString = "";
  std::map<int, Object*>* object_map = m_datacontainer->getObjectsMapPtr();
  Object* root_object = object_map->at(root_hvgx_id);

  if (synapse_param == surrounding_synapses)
  {
    if (root_object->getObjectType() == Object_t::MITO)
    {
      QString root_name = QString(root_object->getName().c_str());

      std::vector<std::pair<int, double>> mito_distance_vec = *root_object->get_distance_vector_ptr();

      std::sort(mito_distance_vec.begin(), mito_distance_vec.end(), sortByVal); // sort by value ascending

      for (auto const& [syn_id, syn_distance] : mito_distance_vec)
      {
        std::vector<std::pair<int, double>> syn_distance_map = *m_datacontainer->getObjectsMapPtr()->at(syn_id)->get_distance_vector_ptr();

        std::sort(syn_distance_map.begin(), syn_distance_map.end(), sortByVal); // sort by value ascending

        bool distance_ok = syn_distance <= distanceThreshold;
        bool type_ok = object_map->at(syn_id)->getObjectType() == Object_t::SYNAPSE;

        if (distance_ok && type_ok)
        {
          QString syn_sub_newick = "";
          QString name = "";

          for (auto const& [mito_id, mito_dist] : syn_distance_map)
          {
            distance_ok = mito_dist < distanceThreshold;
            type_ok = object_map->at(mito_id)->getObjectType() == Object_t::MITO;

            if (distance_ok && type_ok && mito_id != root_hvgx_id)
            {
              QString mito_sub_newick = "";
              name = object_map->at(mito_id)->getName().c_str();
              mito_sub_newick = name + ":" + QString::number(mito_dist);
              syn_sub_newick += mito_sub_newick + ",";
            }
          }

          syn_sub_newick = syn_sub_newick.left(syn_sub_newick.lastIndexOf(","));
          name = object_map->at(syn_id)->getName().c_str();

          if (!syn_sub_newick.isEmpty())
          {
            syn_sub_newick = "(" + syn_sub_newick + ')';
          }
          syn_sub_newick += name + ":" + QString::number(syn_distance);

          newickString += syn_sub_newick + ",";
        }
      }
      newickString = newickString.left(newickString.lastIndexOf(","));
      newickString = "(" + newickString + ")" + root_name;
    }

  }
  else if (synapse_param == related_synapses)
  {
    QString root_name = QString(root_object->getName().c_str());
    Object* parent = object_map->at(root_object->getParentID());

    std::vector<std::pair<int, double>> mito_distance_vec = *root_object->get_distance_vector_ptr();

    std::map<int, double>* mito_distance_map = root_object->get_distance_map_ptr();
    std::vector<std::pair<int, double>> synapes_distances;

    for (auto& syn : *parent->getSynapses())
    {
      double distance_to_root = mito_distance_map->at(syn->getHVGXID());
      synapes_distances.push_back(std::make_pair(syn->getHVGXID(), distance_to_root));
    }

    std::sort(synapes_distances.begin(), synapes_distances.end(), sortByVal); // sort by value ascending

    for (auto const& [syn_id, syn_distance] : synapes_distances)
    {
      QString syn_sub_newick = "";
      QString name = "";

      int cell_id = m_datacontainer->getObjectThatHasSynapse(syn_id, parent->getHVGXID());

      if (cell_id > 0)
      {
        QList<int> mitos = m_datacontainer->getMitosOfCell(cell_id);

        for (int i : mitos)
        {
          Object* mito = object_map->at(i);
          double mito_dist = mito->get_distance_map_ptr()->at(syn_id);
          QString mito_sub_newick = "";
          name = mito->getName().c_str();
          mito_sub_newick = name + ":" + QString::number(mito_dist);
          syn_sub_newick += mito_sub_newick + ",";
        }
      }

      syn_sub_newick = syn_sub_newick.left(syn_sub_newick.lastIndexOf(","));
      name = object_map->at(syn_id)->getName().c_str();

      if (!syn_sub_newick.isEmpty())
      {
        syn_sub_newick = "(" + syn_sub_newick + ')';
      }
      syn_sub_newick += name + ":" + QString::number(syn_distance);

      newickString += syn_sub_newick + ",";
    }
    newickString = newickString.left(newickString.lastIndexOf(","));
    newickString = "(" + newickString + ")" + root_name;
  }

  return newickString;
}

DistanceTreeData::DistanceTreeData(int ID, QString newickString, GlobalVisParameters* global_vis_parameters, DataContainer* data_container)
{
  m_hvgxID = ID;
  m_newickString = newickString;

  m_global_vis_parameters = global_vis_parameters;
  m_datacontainer = data_container;
}

DistanceTreeData::~DistanceTreeData()
{
  // do nothing for now
}

Q_INVOKABLE QString DistanceTreeData::getNewickString()
{
  return Q_INVOKABLE m_newickString;
}

Q_INVOKABLE void DistanceTreeData::setSelectedNode(QString name)
{
  m_selected_node = name;
}

Q_INVOKABLE QString DistanceTreeData::getSelectedNode()
{
  return Q_INVOKABLE m_selected_node;
}

Q_INVOKABLE void DistanceTreeData::setHighlightedStructure(const QString& name)
{
  int hvgx_id = m_datacontainer->getIndexByName(name);
  if (!m_global_vis_parameters->highlighted_objects.contains(hvgx_id))
  {
    m_global_vis_parameters->highlighted_objects.append(hvgx_id);
  }
}

Q_INVOKABLE void DistanceTreeData::removeHighlightedStructure(const QString& name_to_remove)
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

Q_INVOKABLE void DistanceTreeData::removeAllHighlightedStructures()
{
  QVector<int>* highlighted = &m_global_vis_parameters->highlighted_objects;
  highlighted->clear();
}