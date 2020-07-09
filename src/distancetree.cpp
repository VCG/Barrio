#include "distancetree.h"

DistanceTree::DistanceTree(DataContainer* datacontainer)
{
  m_datacontainer = datacontainer;
}

DistanceTree::~DistanceTree()
{
  // destruct distance tree
}

QWebEngineView* DistanceTree::getVisWidget(int ID)
{
  QString newickString = createNewickString(ID, 0.5);
  data = new DistanceTreeData(newickString);

  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("distance_tree_data"), data);
  view->load(getHTMLPath(m_index_filename));

  return view;
}

/*
  hvgxID must be a mitochondrion
*/
QString DistanceTree::createNewickString(int hvgxID, float distanceThreshold)
{
  QString newickString = "";
  std::map<int, Object*>* object_map = m_datacontainer->getObjectsMapPtr();
  Object* object = object_map->at(hvgxID);

  if (object->getObjectType() == Object_t::MITO)
  {
    QString root_name = QString(object->getName().c_str());

    std::map<int, double>* mito_distance_map = object->get_distance_map_ptr();

    for (auto const& [syn_id, syn_distance] : *mito_distance_map)
    {
      std::map<int, double>* syn_distance_map = m_datacontainer->getObjectsMapPtr()->at(syn_id)->get_distance_map_ptr();
      bool distance_ok = syn_distance <= distanceThreshold;
      bool type_ok = object_map->at(syn_id)->getObjectType() == Object_t::SYNAPSE;

      if (distance_ok && type_ok)
      {
        QString syn_sub_newick = "";
        QString name = "";

        for (auto const& [mito_id, mito_dist] : *syn_distance_map)
        {
          distance_ok = mito_dist < distanceThreshold;
          type_ok = object_map->at(mito_id)->getObjectType() == Object_t::MITO;

          if (distance_ok && type_ok)
          {
            QString mito_sub_newick = "";
            name = object_map->at(mito_id)->getName().c_str();
            mito_sub_newick = name + ":" + QString::number(mito_dist);
            syn_sub_newick += mito_sub_newick + ",";
          }
        }

        syn_sub_newick = syn_sub_newick.left(syn_sub_newick.lastIndexOf(","));
        name = object_map->at(syn_id)->getName().c_str();
        syn_sub_newick = "(" + syn_sub_newick + ")" + name + ":" + QString::number(syn_distance);

        newickString += syn_sub_newick + ",";
      }
    }
    newickString = newickString.left(newickString.lastIndexOf(","));
    newickString = "(" + newickString + ")" + root_name;
  }
  else
  {
    qDebug() << "PLEASE SELECT A MITO TO VIEW THE DISTANCE TREE";
  }
  
  return newickString;
}

DistanceTreeData::DistanceTreeData(QString newickString) 
{
  m_newickString = newickString;
}

DistanceTreeData::~DistanceTreeData()
{
  // do nothing for now
}

Q_INVOKABLE QString DistanceTreeData::getNewickString()
{
  return Q_INVOKABLE m_newickString;
}
