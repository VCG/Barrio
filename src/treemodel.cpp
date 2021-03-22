#include "treemodel.h"
#include "treeitem.h"
#include <QFormLayout>

TreeModel::TreeModel(QWidget* parent, DataContainer* datacontainer, MainWidget* mainWidget): QTreeView(parent)
{
  paramList = new QStandardItemModel();
  m_mainwidget = mainWidget;

  paramList->setColumnCount(2);
  paramList->setHeaderData(0, Qt::Horizontal, "Name");
  paramList->setHeaderData(1, Qt::Horizontal, "ID");

  this->setModel(paramList);

  this->setColumnWidth(0, 200);
  this->setColumnWidth(1, 100);

  std::vector<Object*> mitos = datacontainer->getObjectsByType(Object_t::MITO);
  QStringList names = QStringList();

  for (int i = 0; i < mitos.size(); i++)
  {
    names.append(mitos[i]->getName().c_str());
  }

  names.sort();

  QStandardItem* mouse2 = new QStandardItem("Mouse 2");
  QStandardItem* mouse3 = new QStandardItem("Mouse 3");

  paramList->appendRow(mouse2);
  paramList->appendRow(mouse3);

  //std::map<int, Object*>* objects_map = datacontainer->getObjectsMapPtr();
  for (int i = 0; i < names.size(); i++)
  {
    //Object* object_p = (*iter).second;
    Object* object = datacontainer->getObjectByName(names[i]);

    //if (object_p->getObjectType() != Object_t::MITO)
    //{
      QString name(object->getName().c_str());
      QStandardItem* item_name = new QStandardItem(name);
      item_name->setEditable(false);


      int id = object->getHVGXID();
      QStandardItem* item_id = new QStandardItem(QString::number(id));
      item_id->setEditable(false);

      if (name.endsWith("2"))
      {
        mouse2->appendRow(QList<QStandardItem*>() << item_name << item_id);
      }
      else if(name.endsWith("3"))
      {
        mouse3->appendRow(QList<QStandardItem*>() << item_name << item_id);
      }

      /*std::vector<int>* childrenIDs = object->getChildrenIDs();
      int j = 0;
      for (int i=0; i < childrenIDs->size(); i++)
      {
        Object* child = mitos->at(childrenIDs->at(i));
        QString child_name(child->getName().c_str());
        QStandardItem* item_child_name = new QStandardItem(child_name);
        item_child_name->setEditable(false);

        int child_id = child->getHVGXID();
        QStandardItem* child_item_id = new QStandardItem(QString::number(child_id));
        child_item_id->setEditable(false);

        item_name->appendRow(QList<QStandardItem*>() << item_child_name << child_item_id);

        j++;
      }*/
      //i++;
    //}
  }

 
}

TreeModel::~TreeModel()
{

}

void TreeModel::selectItem(const QModelIndex& index)
{
  //extracting hvgx id
  int hvgx = index.siblingAtColumn(1).data().toInt();
  m_mainwidget->addWidgetGroup(hvgx, false);

  QList<QStandardItem*> items;
  
  QModelIndex c0 = index.sibling(index.row(), 0);
  QModelIndex c1 = index.sibling(index.row(), 1);

  QStandardItemModel* sModel = qobject_cast<QStandardItemModel*>(this->model());
  QStandardItem* col0 = sModel->itemFromIndex(c0);
  QStandardItem* col1 = sModel->itemFromIndex(c1);

  col0->setBackground(QColor(161, 255, 186));
  col1->setBackground(QColor(161, 255, 186));

  items.append(col0);
  items.append(col1);

  m_mainwidget->addStandardItem(hvgx, items);
}

