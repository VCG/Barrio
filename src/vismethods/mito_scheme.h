#ifndef MITO_SCHEME_H
#define MITO_SCHEME_H

#include <QChar>
#include "vismethod.h"
#include "../globalParameters.h"
#include "../datacontainer.h"

class MitoSchemeData : public QObject
{
  Q_OBJECT

public:
  MitoSchemeData(int ID, QString mito_name, DataContainer* data_container, GlobalVisParameters* global_vis_parameters);
  ~MitoSchemeData();

  Q_INVOKABLE QString getData();
  Q_PROPERTY(QString json_string READ getData);

  Q_INVOKABLE int getID();
  Q_PROPERTY(int hvgx_id READ getID);

  Q_INVOKABLE QString getMitoName();
  Q_PROPERTY(QString mito_name READ getMitoName);

  void setJSONString(QString json) { m_json_string = json; };

  int m_hvgxID;
  QString m_mito_name;
  QString m_json_string;

  GlobalVisParameters* m_global_vis_parameters;
  DataContainer* m_datacontainer;

  Q_INVOKABLE void setHighlightedStructure(const int parentID, int spineNumber);
  Q_INVOKABLE void removeHighlightedStructure(const int parentID, int spineNumber);
};

class MitoScheme : public IVisMethod
{
public:
  MitoScheme(MitoScheme* mitoScheme);
  MitoScheme(GlobalVisParameters* visparams, DataContainer* datacontainer);
  ~MitoScheme();

  QString createJSONString(QList<int>* selectedObjects);

  //inherited functions
  QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
  QWebEngineView* getWebEngineView();
  bool            update();
  MitoScheme*     clone();
  bool            update_needed();

  VisType         getType();

  void            setSpecificVisParameters(SpecificVisParameters params);

private:

  MitoSchemeData* m_data;
  DataContainer* m_datacontainer;

  QString m_title;
  QString m_index_filename = "web/mito_scheme_index.html";

  GlobalVisParameters* m_global_vis_parameters;
  QWebEngineView* m_web_engine_view;
};

#endif


