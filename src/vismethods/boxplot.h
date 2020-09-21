#ifndef BOXPLOT_H
#define BOXPLOT_H

#include <QChar>
#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class BoxplotData : public QObject
{
  Q_OBJECT

public:
  BoxplotData(QString json_data, DataContainer* data_container, GlobalVisParameters* global_vis_parameters);
  ~BoxplotData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  Q_INVOKABLE QString getColormap();
  Q_PROPERTY(QString colormap READ getColormap);

  void setJSONString(QString json) { m_json_string = json; };
  void setColors(QString colors) { m_colors = colors; };

  int m_hvgxID;
  QString m_json_string;
  QString m_colors;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;
};

class Boxplot : public IVisMethod
{
public:
  Boxplot(Boxplot* boxplot);
  Boxplot(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~Boxplot();

  QString createJSONString(QList<int>* selectedObjects);

  //inherited functions
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  Boxplot*        clone();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  BoxplotData* m_data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/boxplot_index.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif

