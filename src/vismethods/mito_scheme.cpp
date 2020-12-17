#include "mito_scheme.h"


MitoSchemeData::MitoSchemeData(int ID, DataContainer* data_container, GlobalVisParameters* global_vis_parameters)
{
  m_json_string = data_container->m_sematic_skeleton_json;
  m_hvgxID = ID;
  m_datacontainer = data_container;
  m_global_vis_parameters = global_vis_parameters;
}
MitoSchemeData::~MitoSchemeData()
{
}
Q_INVOKABLE QString MitoSchemeData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE QString MitoSchemeData::getID()
{
  qDebug() << m_hvgxID;
  return Q_INVOKABLE m_hvgxID;
}

MitoScheme::MitoScheme(MitoScheme* mitoScheme)
{
  m_datacontainer = mitoScheme->m_datacontainer;
  m_global_vis_parameters = mitoScheme->m_global_vis_parameters;
}

MitoScheme::MitoScheme(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

MitoScheme::~MitoScheme()
{
}

QString MitoScheme::createJSONString(QList<int>* selectedObjects)
{
  return QString();
}

QWebEngineView* MitoScheme::initVisWidget(int ID, SpecificVisParameters params)
{
  m_data = new MitoSchemeData(ID, m_datacontainer, m_global_vis_parameters);

  setSpecificVisParameters(params);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("mito_scheme_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* MitoScheme::getWebEngineView()
{
  return m_web_engine_view;
}

bool MitoScheme::update()
{
  m_data->setJSONString(m_datacontainer->m_sematic_skeleton_json);
  return true;
}
MitoScheme* MitoScheme::clone()
{
  return new MitoScheme(this);
}

VisType MitoScheme::getType()
{
  return VisType::MITO_SCHEME;
}

void MitoScheme::setSpecificVisParameters(SpecificVisParameters params)
{
}
