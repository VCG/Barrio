#include "javascriptproxy.h"

BarChartSharedData::BarChartSharedData(QObject* parent) : QObject(parent)
{
  m_data.append(0.7);
  m_data.append(0.8);
  m_data.append(0.9);

  m_labels.append("Syn1");
  m_labels.append("Syn2");
  m_labels.append("Syn3");
}

Q_INVOKABLE QList<float> BarChartSharedData::getData()
{
  return Q_INVOKABLE m_data;
}

Q_INVOKABLE QList<QString> BarChartSharedData::getLabels()
{
  return Q_INVOKABLE m_labels;
}
