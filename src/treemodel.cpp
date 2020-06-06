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
      
      paramList.appendRow(QList<QStandardItem*>() << item_name << item_id);

      std::vector<Object*> children = object_p->getChildren();
      int j = 0;
      for (Object* child : children)
      {

        QString child_name(child->getName().c_str());
        QStandardItem* item_child_name = new QStandardItem(child_name);
        //item_name->setChild(j, item_child_name);

        int child_id = child->getHVGXID();
        QStandardItem* child_item_id = new QStandardItem(QString::number(child_id));
        //item_id->setChild(j, child_item_id);

        item_name->appendRow(QList<QStandardItem*>() << item_child_name << child_item_id);

        j++;
      }
      //paramList.setItem(i, 0, item_name);
      //paramList.setItem(i, 1, item_id);
      i++;
    }

    //QStandardItemModel model;
    //QStandardItem* parent0 = new QStandardItem("Parent 0");
    //QStandardItem* col1 = new QStandardItem("1");
    //model.appendRow(QList<QStandardItem*>() << parent0 << col1);
    //// make some children                                                                                                          
    //for (int i = 0; i < 3; ++i) {
    //  QStandardItem* child0 = new QStandardItem(QString("Child %1").arg(i));
    //  QStandardItem* child1 = new QStandardItem(QString("%1").arg(i));
    //  parent0->appendRow(QList<QStandardItem*>() << child0 << child1);
    //}
    
  }
}

