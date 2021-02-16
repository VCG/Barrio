#include "mito_scheme.h"


MitoSchemeData::MitoSchemeData(int parent_id, QString mito_name, DataContainer* data_container, GlobalVisParameters* global_vis_parameters)
{
  m_json_string = data_container->m_sematic_skeleton_json;
  m_hvgxID = parent_id;
  m_mito_name = mito_name;
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

Q_INVOKABLE int MitoSchemeData::getID()
{
  return Q_INVOKABLE m_hvgxID;
}

Q_INVOKABLE QString MitoSchemeData::getMitoName()
{
  qDebug() << m_mito_name;
  return Q_INVOKABLE m_mito_name;
}


Q_INVOKABLE void MitoSchemeData::setHighlightedStructure(const int parentID, int spineNumber)
{
  Object* parent = m_datacontainer->getObjectsMapPtr()->at(parentID);
  std::vector<int>* children_ids = parent->getChildrenIDs();
  for (int i = 0; i < children_ids->size(); i++)
  {
    int id = children_ids->at(i);
    Object* child = m_datacontainer->getObjectsMapPtr()->at(id);
    QString name = QString::fromUtf8(child->getName().c_str());
    int my_spine_number = name.split("_")[2].toInt(); // spine number is contained on the 2 position in the array

    if (my_spine_number == spineNumber && !m_global_vis_parameters->highlighted_objects.contains(id))
    {
      m_global_vis_parameters->highlighted_objects.append(id);
    }
  }
  
}

Q_INVOKABLE void MitoSchemeData::removeHighlightedStructure(const int parentID, int spineNumber)
{
  int hvgx_id = 0;

  Object* parent = m_datacontainer->getObjectsMapPtr()->at(parentID);
  std::vector<int>* children_ids = parent->getChildrenIDs();
  for (int i = 0; i < children_ids->size(); i++)
  {
    int id = children_ids->at(i);
    Object* child = m_datacontainer->getObjectsMapPtr()->at(id);
    QString name = QString::fromUtf8(child->getName().c_str());
    int my_spine_number = name.split("_")[2].toInt(); // spine number is contained on the 2 position in the array

    if (my_spine_number == spineNumber)
    {
      hvgx_id = id;
    }
  }

  
  QVector<int>* highlighted = &m_global_vis_parameters->highlighted_objects;
  if (highlighted->contains(hvgx_id))
  {
    QMutableVectorIterator<int> it(*highlighted);
    while (it.hasNext())
    {
      if (it.next() == hvgx_id) {
        it.remove();
      }
    }
  }
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
  int parentID = m_datacontainer->getObjectsMapPtr()->at(ID)->getParentID();
  QString mito_name = m_datacontainer->getObjectsMapPtr()->at(ID)->getName().c_str();
  m_data = new MitoSchemeData(parentID, mito_name, m_datacontainer, m_global_vis_parameters);

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

bool MitoScheme::update_needed()
{
    return false;
}

VisType MitoScheme::getType()
{
  return VisType::MITO_SCHEME;
}

void MitoScheme::setSpecificVisParameters(SpecificVisParameters params)
{
}
