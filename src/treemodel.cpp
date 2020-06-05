#include "treemodel.h"
#include "treeitem.h"

TreeModel::TreeModel()
{
  
}

TreeModel::~TreeModel()
{

}

void TreeModel::init(QTreeView* tree, DataContainer* datacontainer)
{
  paramList.setColumnCount(2);
  paramList.setHeaderData(0, Qt::Horizontal, "Name");
  paramList.setHeaderData(1, Qt::Horizontal, "ID");
  tree->setModel(&paramList);


  std::map<int, Object*>* objects_map = datacontainer->getObjectsMapPtr();

  int i = 0;
  for (auto iter = objects_map->rbegin(); iter != objects_map->rend(); iter++)
  {
    Object* object_p = (*iter).second;

    if (object_p->getObjectType() != Object_t::MITO) 
    {
      QString name(object_p->getName().c_str());
      QStandardItem* item_name = new QStandardItem(name);
      int id = object_p->getHVGXID();
      QStandardItem* item_id = new QStandardItem(QString::number(id));
      

      /*std::vector<Object*> children = object_p->getChildren();
      int j = 0;
      for (Object* child : children)
      {
        QString child_name(child->getName().c_str());
        QStandardItem* item1 = new QStandardItem(child_name);
        itemroot->setChild(j, item1);
        j++;
      }*/
      paramList.setItem(i, 0, item_name);
      paramList.setItem(i, 1, item_id);
      i++;
    }
    
  }
}

