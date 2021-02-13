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

Q_INVOKABLE QString ScatterplotData::getXAxis()
{
  return Q_INVOKABLE QString("Minimum distance to cell");
}

Q_INVOKABLE QString ScatterplotData::getYAxis()
{
  return Q_INVOKABLE QString("Percentage close than bla");
}

Q_INVOKABLE QString ScatterplotData::getXUnit()
{
  return Q_INVOKABLE QString("microns");
}

Q_INVOKABLE QString ScatterplotData::getYUnit()
{
  return Q_INVOKABLE QString("microns");
}

Q_INVOKABLE void ScatterplotData::selectStructure(const QString& name)
{
  int hvgx = m_datacontainer->getIndexByName(name);
  m_global_vis_parameters->my_add_queue.append(hvgx);
  return Q_INVOKABLE void();
}

Q_INVOKABLE void ScatterplotData::removeStructure(const QString& name)
{
  qDebug() << name; 
  return Q_INVOKABLE void();
}

Q_INVOKABLE void ScatterplotData::setHighlightedFrame(const QString& name)
{
  int hvgx = m_datacontainer->getIndexByName(name);
  if (!m_global_vis_parameters->highlighted_group_boxes.contains(hvgx))
  {
    m_global_vis_parameters->highlighted_group_boxes.append(hvgx);
  }
  
  return Q_INVOKABLE void();
}

Q_INVOKABLE void ScatterplotData::removeHighlightedFrame(const QString& name_to_remove)
{
  int hvgx_id = m_datacontainer->getIndexByName(name_to_remove);
  QVector<int>* highlighted = &m_global_vis_parameters->highlighted_group_boxes;
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
  return Q_INVOKABLE void();
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
  std::vector<Object*> mitochondria = m_datacontainer->getObjectsByType(Object_t::MITO);

  QJsonArray document;
  for (auto mito : mitochondria)
  {
    QJsonObject object_json;

    std::vector<int>* mito_indices = mito->get_indices_list();
    std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

    float minimum = 1000.0;
    float counter = 0.0;
    float threshold = 0.05;
    float meshSize = 0.0;

    for (auto j : *mito_indices)
    {
      VertexData vertex = vertices->at(j);
      float distance = (float)vertex.distance_to_cell;

      if (distance > 100) continue; // filter boarder values

      if (distance < minimum)
      {
        minimum = distance;
      }

      if (distance < threshold)
      {
        counter++;
      }

      meshSize++;
    }

    float perc = counter / meshSize;

    object_json.insert("name", mito->getName().c_str());
    object_json.insert("min", minimum);
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

bool Scatterplot::update_needed()
{
  return false;
}

VisType Scatterplot::getType()
{
    return VisType::SCATTERPLOT;
}

void Scatterplot::setSpecificVisParameters(SpecificVisParameters params)
{
}

