#include "barchart.h"
#include <filesystem>

namespace fs = std::filesystem;

Barchart::Barchart()
{
  m_values.append(0.7);
  m_values.append(0.8);
  m_values.append(0.9);

  m_labels.append("Syn1");
  m_labels.append("Syn2");
  m_labels.append("Syn3");
}

Barchart::~Barchart()
{
  // destruct
}

Q_INVOKABLE QList<float> Barchart::getData()
{
  return Q_INVOKABLE m_values;
}

Q_INVOKABLE QList<QString> Barchart::getLabels()
{
  return Q_INVOKABLE m_labels;
}

QWebEngineView* Barchart::getVisWidget()
{
  fs::path current_path = fs::current_path();
  QString base_path = QString(current_path.parent_path().string().c_str());
  base_path.replace("\\", "/");
  QString url = base_path + QString("/src/barchart_index.html");

  QWebEngineView* view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(view->page());
  view->page()->setWebChannel(channel);
  channel->registerObject(QString("data"), this);
  view->load(QUrl(url));

  return view;
}