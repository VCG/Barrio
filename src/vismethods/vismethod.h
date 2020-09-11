#ifndef VISMETHOD_H
#define VISMETHOD_H

#include <QString>
#include <QWebEngineView>
#include <QObject>
#include <QResource>
#include <QFile>
#include <filesystem>

#include <QWebChannel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

#include "../datacontainer.h"

namespace fs = std::filesystem;

enum class VisType { DISTANCE_TREE = 0, BARCHART = 1, DISTANCE_MATRIX = 2, HISTOGRAM = 3, BOXPLOT = 4, SCATTERPLOT = 5 };

class IVisMethod: public QObject
{
  Q_OBJECT
public:

  virtual QWebEngineView* initVisWidget(int ID, SpecificVisParameters params) = 0;
  virtual bool            update() = 0;
  virtual QWebEngineView* getWebEngineView() = 0;
  virtual IVisMethod*     clone() = 0;
  virtual VisType         getType() = 0;
  virtual void            setSpecificVisParameters(SpecificVisParameters params) = 0;

  QUrl getHTMLPath(QString filename) 
  {
    fs::path current_path = fs::current_path();
    QString base_path = QString(current_path.parent_path().string().c_str());
    base_path.replace("\\", "/");
    QString url = base_path + QString("/src/") + filename;

    return QUrl(url);
  }

protected:
  //QString           method_name;
  QString           m_index_filename;
  QWebEngineView*   m_web_engine_view;
};

#endif

