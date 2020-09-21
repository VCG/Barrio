#include "boxplot.h"

Boxplot::Boxplot(Boxplot* boxplot)
{
  m_datacontainer = boxplot->m_datacontainer;
  m_global_vis_parameters = boxplot->m_global_vis_parameters;
}

Boxplot::Boxplot(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Boxplot::~Boxplot()
{
}

QString Boxplot::createJSONString(QList<int>* selectedObjects)
{
  QJsonArray document;
  for (auto i : *selectedObjects)
  {
    Object* mito = m_datacontainer->getObject(i);

    std::vector<int>* mito_indices = mito->get_indices_list();
    std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

    QJsonObject mito_json;
    QJsonArray  mito_distances;
    for (auto j : *mito_indices)
    {
      VertexData vertex = vertices->at(j);
      double distance_to_cell = vertex.distance_to_cell;
      mito_distances.push_back(QJsonValue::fromVariant(distance_to_cell));
    }
    mito_json.insert("key", mito->getName().c_str());
    mito_json.insert("value", mito_distances);
    document.push_back(mito_json);
  }
  QJsonDocument doc(document);
  return doc.toJson(QJsonDocument::Indented);
}

QWebEngineView* Boxplot::initVisWidget(int ID, SpecificVisParameters params)
{
  QString json = createJSONString(&m_global_vis_parameters->selected_objects);
  m_data = new BoxplotData(json, m_datacontainer, m_global_vis_parameters);

  setSpecificVisParameters(params);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("boxplot_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* Boxplot::getWebEngineView()
{
  return m_web_engine_view;
}

bool Boxplot::update()
{
  QString json = createJSONString(&m_global_vis_parameters->selected_objects);
  m_data->setJSONString(json);
  return true;
}

Boxplot* Boxplot::clone()
{
  return new Boxplot(this);
}

VisType Boxplot::getType()
{
    return VisType::BOXPLOT;
}

void Boxplot::setSpecificVisParameters(SpecificVisParameters params)
{
  m_data->setColors(params.colors);
}

BoxplotData::BoxplotData(QString json_data, DataContainer* data_container, GlobalVisParameters* global_vis_parameters)
{
  m_json_string = json_data;
  m_datacontainer = data_container;
  m_global_vis_parameters = global_vis_parameters;
}

BoxplotData::~BoxplotData()
{
}

Q_INVOKABLE QString BoxplotData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE QString BoxplotData::getColormap()
{
  return Q_INVOKABLE m_colors;
}
