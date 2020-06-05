#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant>& data, TreeItem* parentItem)
  :m_itemData(data), m_parentItem(parentItem)
{

}

TreeItem::~TreeItem()
{
}

void TreeItem::appendChild(TreeItem* child)
{
  m_childItems.append(child);
}

TreeItem* TreeItem::child(int row)
{
  return nullptr;
}

int TreeItem::childCount() const
{
  return 0;
}

int TreeItem::columnCount() const
{
  return 0;
}

QVariant TreeItem::data(int column) const
{
  return QVariant();
}

int TreeItem::row() const
{
  return 0;
}

TreeItem* TreeItem::parentItem()
{
  return nullptr;
}
