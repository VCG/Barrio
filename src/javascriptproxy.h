#ifndef JAVASCRIPTPROXY_H
#define JAVASCRIPTPROXY_H

#include <QObject>
#include <QList>

class BarChartSharedData : public QObject
{
  Q_OBJECT

public:
  BarChartSharedData(QObject* parent = nullptr);

  Q_INVOKABLE QList<float> getData();
  Q_PROPERTY(QList<float> values READ getData);

  Q_INVOKABLE QList<QString> getLabels();
  Q_PROPERTY(QList<QString> labels READ getLabels);


private:
  QList<float>    m_data;
  QList<QString>  m_labels;
};

#endif