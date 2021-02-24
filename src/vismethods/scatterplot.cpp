#include "scatterplot.h"

ScatterplotData::ScatterplotData(QString json, QString selected_objects_json, GlobalVisParameters* global_vis_parameters, DataContainer* data_container)
{
  m_json_string = json;
  m_selected_structures_json = selected_objects_json;
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
  return Q_INVOKABLE m_x_axis_label;
}

Q_INVOKABLE QString ScatterplotData::getYAxis()
{
  return Q_INVOKABLE m_y_axis_label;
}

Q_INVOKABLE QString ScatterplotData::getXUnit()
{
  return Q_INVOKABLE QString("microns");
}

Q_INVOKABLE QString ScatterplotData::getYUnit()
{
  return Q_INVOKABLE QString("microns");
}

Q_INVOKABLE QString ScatterplotData::getSelectedStructures()
{
  return Q_INVOKABLE m_selected_structures_json;
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

QString Scatterplot::createJSONString()
{
  std::vector<Object*> mitochondria = m_datacontainer->getObjectsByType(Object_t::MITO);

  QJsonObject x_axis = m_settings.value("x-axis").toObject();
  QString x_attribute = x_axis.value("attribute").toString();

  QJsonObject y_axis = m_settings.value("y-axis").toObject();
  QString y_attribute = y_axis.value("attribute").toString();

  QJsonDocument skeleton_doc = QJsonDocument::fromJson(m_datacontainer->m_sematic_skeleton_json.toUtf8());
  QJsonArray cell_array = skeleton_doc.array();

  QMap<QString, float> x_values;
  QMap<QString, float> y_values;

  QString mito_spine_coverage = "mito-spine-coverage";
  QString mito_volume = "mito-volume";
  QString min_distance_to_cell = "min-distance-to-cell";
  QString surf_percentage = "surf-percentage";

  if (x_attribute == mito_spine_coverage || y_attribute == mito_spine_coverage)
  {
    for (auto c : cell_array) 
    {
      QJsonObject cell = c.toObject();
      QJsonArray spines = cell.value("spines").toArray();
      QJsonArray mitos = cell.value("mitochondria").toArray();

      for (auto m : mitos) {
        QJsonObject mito = m.toObject();
        QString name = mito.value("name").toString();
        float spine_coverage = getMitoSpineCoverage(mito, spines);

        if (x_attribute == mito_spine_coverage) {
          x_values.insert(name, spine_coverage);
        }
        else if (y_attribute == mito_spine_coverage) {
          y_values.insert(name, spine_coverage);
        }
      }
    }
  } 
  
  if (x_attribute == mito_volume || y_attribute == mito_volume)
  {
    for (auto mito : mitochondria)
    {
      QString name = mito->getName().c_str();
      float volume = mito->getVolume();

      qDebug() << volume;

      if (x_attribute == mito_volume) {
        x_values.insert(name, volume);
      }
      else if (y_attribute == mito_volume) {
        y_values.insert(name, volume);
      }
    }
  }

  if (x_attribute == min_distance_to_cell || y_attribute == min_distance_to_cell)
  {
    for (auto mito : mitochondria)
    {
      QJsonObject object_json;

      QString name = mito->getName().c_str();
      std::vector<int>* mito_indices = mito->get_indices_list();
      std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

      float minimum = 100.0;
      int counter = 0;

      for (auto j : *mito_indices)
      {
        VertexData vertex = vertices->at(j);
        float distance = (float)vertex.distance_to_cell;

        if (distance > 100) continue; // filter boarder values

        if (distance < minimum)
        {
          minimum = distance;
        }
        counter++;
      }

      if (counter > 0 && x_attribute == min_distance_to_cell) {
        x_values.insert(name, minimum);
      }
      else if (counter > 0 && y_attribute == min_distance_to_cell) {
        y_values.insert(name, minimum);
      }
    }
  }


  if (x_attribute == surf_percentage || y_attribute == surf_percentage)
  {
    float threshold = 1.0;
    if (x_attribute == surf_percentage)
    {
      QJsonObject xaxis = m_settings.value("x-axis").toObject();
      threshold = xaxis.value("threshold").toVariant().toFloat();
    }
    else if(y_attribute == surf_percentage)
    {
      QJsonObject yaxis = m_settings.value("y-axis").toObject();
      threshold = yaxis.value("threshold").toVariant().toFloat();
    }

    for (auto mito : mitochondria)
    {
      QJsonObject object_json;

      QString name = mito->getName().c_str();
      std::vector<int>* mito_indices = mito->get_indices_list();
      std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

      float minimum = 1000.0;
      float counter = 0.0;
      //float threshold = 0.05;
      float meshSize = 0.0;

      for (auto j : *mito_indices)
      {
        VertexData vertex = vertices->at(j);
        float distance = (float)vertex.distance_to_cell;

        if (distance > 100) continue; // filter boarder values

        if (distance < threshold)
        {
          counter++;
        }

        meshSize++;
      }

      float perc = counter / meshSize;

      if (x_attribute == surf_percentage) {
        x_values.insert(name, perc);
      }
      else if (y_attribute == surf_percentage) {
        y_values.insert(name, perc);
      }
    }
  }

  QJsonArray document;
  for (auto mito : mitochondria)
  {
    QString name = mito->getName().c_str();
    QJsonObject object_json;

    if (x_values.contains(name) && y_values.contains(name))
    {
      object_json.insert("name", name);
      object_json.insert("x", x_values.value(name));
      object_json.insert("y", y_values.value(name));

      document.push_back(object_json);
    }    
  }

  QJsonDocument doc(document);
  return doc.toJson(QJsonDocument::Indented);
}

QString Scatterplot::createSelectedObjectsJSON(QList<int>* selectedObjects)
{
  QJsonArray document;
  for (const auto& id : *selectedObjects)
  {
    QString name = m_datacontainer->getObject(id)->getName().c_str();
    document.push_back(name);
  }
  QJsonDocument doc(document);
  return doc.toJson(QJsonDocument::Indented);
}

float Scatterplot::getMitoSpineCoverage(QJsonObject mito, QJsonArray spines)
{
  float mito_start = mito.value("start").toVariant().toFloat();
  float mito_end = mito.value("end").toVariant().toFloat();
  float coverage_counter = 0.0;

  for (auto s : spines) {
    QJsonObject spine = s.toObject();
    float spine_start = spine.value("start").toVariant().toFloat();

    if (mito_start <= spine_start && spine_start <= mito_end)
    {
      coverage_counter += 1.0;
    }
  }
  return coverage_counter;
}

QWebEngineView* Scatterplot::initVisWidget(int ID, SpecificVisParameters params)
{
  m_settings = params.settings; 
  
  QString json = createJSONString();
  QString selected_objects_json = createSelectedObjectsJSON(&m_global_vis_parameters->selected_objects);
  
  m_data = new ScatterplotData(json, selected_objects_json, m_global_vis_parameters, m_datacontainer);

  QJsonObject x_axis = m_settings.value("x-axis").toObject();
  QString x_label = x_axis.value("label").toString();
  m_data->setXLabel(x_label);

  QJsonObject y_axis = m_settings.value("y-axis").toObject();
  QString y_label = y_axis.value("label").toString();
  m_data->setYLabel(y_label);

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
  QString json = createJSONString();
  m_data->setJSONString(json);

  QString selected_structures_json = createSelectedObjectsJSON(&m_global_vis_parameters->selected_objects);
  m_data->setSelectedStructures(selected_structures_json);

  return true;
}

Scatterplot* Scatterplot::clone()
{
  return new Scatterplot(this);
}

bool Scatterplot::update_needed()
{
  return true;
}

VisType Scatterplot::getType()
{
    return VisType::SCATTERPLOT;
}

void Scatterplot::setSpecificVisParameters(SpecificVisParameters params)
{
}

