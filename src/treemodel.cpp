#include "treemodel.h"
#include "treeitem.h"
#include <QFormLayout>

TreeModel::TreeModel(QWidget* parent, DataContainer* datacontainer, MainWidget* mainWidget): QTreeView(parent)
{
  paramList = new QStandardItemModel();
  m_mainwidget = mainWidget;
  m_datacontainer = datacontainer;

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

  for (int i = 0; i < names.size(); i++)
  {
    Object* object = datacontainer->getObjectByName(names[i]);

      QString name(object->getName().c_str());
      QStandardItem* item_name = new QStandardItem(name);
      item_name->setEditable(false);

      int id = object->getHVGXID();
      QStandardItem* item_id = new QStandardItem(QString::number(id));
      item_id->setEditable(false);

      paramList->appendRow(QList<QStandardItem*>() << item_name << item_id);
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

  QColor color_selected(195, 147, 226);
  QColor color_close(202, 205, 143);
  QColor color_reset(255, 255, 255);

  clearCloseColorMarking(color_close, color_reset);


  //QList<QStandardItem*> items;
  //QStandardItem* col0 = paramList->item(index.row(), 0);
  //QStandardItem* col1 = paramList->item(index.row(), 1);

  //QColor background_color_purple(195, 147, 226);
  //QColor background_color_green(202, 205, 143);

  //col0->setBackground(background_color_green);
  //col1->setBackground(background_color_green);

  //items.append(col0);
  //items.append(col1);

  QList<QStandardItem*> items = setBackgroundColor(index, color_selected);
  m_mainwidget->addStandardItem(hvgx, items);

  QStandardItemModel* sModel = qobject_cast<QStandardItemModel*>(this->model());
  for (int r = 0; r < sModel->rowCount(); r++) 
  {
    QModelIndex object_index = sModel->index(r, 0);
    int potentially_close_structure = object_index.siblingAtColumn(1).data().toInt();
    bool isClose = m_datacontainer->isWithinDistance(hvgx, potentially_close_structure, CONSIDER_CLOSE);
    if (isClose && hvgx != potentially_close_structure)
    {
       setBackgroundColor(object_index, color_close);
       close_indices.append(potentially_close_structure);
    }
  }
}

QList<QStandardItem*> TreeModel::setBackgroundColor(const QModelIndex& index, QColor color)
{
    QList<QStandardItem*> items;

    QModelIndex c0 = index.sibling(index.row(), 0);
    QModelIndex c1 = index.sibling(index.row(), 1);

    QStandardItemModel* sModel = qobject_cast<QStandardItemModel*>(this->model());
    QStandardItem* col0 = sModel->itemFromIndex(c0);
    QStandardItem* col1 = sModel->itemFromIndex(c1);

    col0->setBackground(color);
    col1->setBackground(color);

    items.append(col0);
    items.append(col1);

    return items;
}

void TreeModel::clearCloseColorMarking(QColor close_color, QColor reset_color)
{
    QStandardItemModel* sModel = qobject_cast<QStandardItemModel*>(this->model());
    for (int r = 0; r < sModel->rowCount(); r++) {
        QModelIndex index = sModel->index(r, 0);
        if (sModel->hasChildren(index))
        {
            for (int i = 0; i < sModel->rowCount(index); i++)
            {
                QModelIndex object_index = sModel->index(i, 1, index);
                int hvgx_id = object_index.data().toInt();
                if (close_indices.contains(hvgx_id))
                {
                    setBackgroundColor(object_index, reset_color);
                }
            }
        }
    }
    close_indices.clear();
}

void TreeModel::resetCloseColor(const QModelIndex& index, QColor close_color, QColor reset_color)
{
    QList<QStandardItem*> items;

    QModelIndex c0 = index.sibling(index.row(), 0);
    QModelIndex c1 = index.sibling(index.row(), 1);

    QStandardItemModel* sModel = qobject_cast<QStandardItemModel*>(this->model());
    QStandardItem* col0 = sModel->itemFromIndex(c0);
    QStandardItem* col1 = sModel->itemFromIndex(c1);

    if (col0->background() == close_color)
    {
        col0->setBackground(reset_color);
    }
    if (col1->background() == close_color)
    {
        col1->setBackground(reset_color);
    }

    items.append(col0);
    items.append(col1);
}

