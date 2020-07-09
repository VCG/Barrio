#ifndef VISMETHOD_H
#define VISMETHOD_H

#include <QString>
#include <QWebEngineView>
#include <QObject>
#include <QResource>
#include <QFile>
#include <filesystem>

#include "datacontainer.h"

namespace fs = std::filesystem;

class IVisMethod: public QObject
{
  Q_OBJECT
public:

  virtual QWebEngineView* getVisWidget(int ID) = 0;

  QUrl getHTMLPath(QString filename) 
  {
    fs::path current_path = fs::current_path();
    QString base_path = QString(current_path.parent_path().string().c_str());
    base_path.replace("\\", "/");
    QString url = base_path + QString("/src/") + filename;

    return QUrl(url);
  }

protected:
  QString m_index_filename;
};

#endif

