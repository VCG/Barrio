#include "scatterplot.h"

ScatterplotData::ScatterplotData(QString json, GlobalVisParameters* global_vis_parameters, DataContainer* data_container)
{
  m_json_string = json;
  m_global_vis_parameters = global_vis_parameters;
  m_datacontainer = data_container;
}

ScatterplotData::~ScatterplotData()
{
}

Q_INVOKABLE QString ScatterplotData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Scatterplot::Scatterplot(Scatterplot* scatterplot)
{
  m_datacontainer = scatterplot->m_datacontainer;
  m_global_vis_parameters = scatterplot->m_global_vis_parameters;
}

Scatterplot::Scatterplot(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Scatterplot::~Scatterplot()
{
}

QString Scatterplot::createJSONString(QList<int>* selectedObjects)
{
  QJsonArray document;
  for (auto i : *selectedObjects)
  {
    QJsonObject object_json;

    Object* object = m_datacontainer->getObjectsMapPtr()->at(i);
    std::vector<int>* mito_indices = object->get_indices_list();
    std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

    float minimum = 1000;
    float counter = 0.0;
    float threshold = 0.1;

    for (auto j : *mito_indices)
    {
      VertexData vertex = vertices->at(j);
      if(vertex.distance_to_cell < minimum);
      {
        minimum = vertex.distance_to_cell;
      }

      if (vertex.distance_to_cell <= threshold)
      {
        counter += 1.0;
      }
    }
   
    float perc = counter / (float)mito_indices->size();

    object_json.insert("name", object->getName().c_str());
    object_json.insert("avg", minimum);
    object_json.insert("perc", perc);

    document.push_back(object_json);
  }

  QJsonDocument doc(document);
  return doc.toJson(QJsonDocument::Indented);
}

QWebEngineView* Scatterplot::initVisWidget(int ID, SpecificVisParameters params)
{
  QString json = createJSONString(&m_global_vis_parameters->selected_objects);
  m_data = new ScatterplotData(json, m_global_vis_parameters, m_datacontainer);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("scatterplot_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* Scatterplot::getWebEngineView()
{
  return m_web_engine_view;
}

bool Scatterplot::update()
{
  QString json = createJSONString(&m_global_vis_parameters->selected_objects);
  m_data->setJSONString(json);
  return true;
}

Scatterplot* Scatterplot::clone()
{
  return new Scatterplot(this);
}

VisType Scatterplot::getType()
{
    return VisType::SCATTERPLOT;
}

void Scatterplot::setSpecificVisParameters(SpecificVisParameters params)
{
}

