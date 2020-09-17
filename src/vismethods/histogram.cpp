#include "histogram.h"

Histogram::Histogram(Histogram* histogram)
{
  m_datacontainer = histogram->m_datacontainer;
  m_global_vis_parameters = histogram->m_global_vis_parameters;
}

Histogram::Histogram(GlobalVisParameters* visparams, DataContainer* datacontainer)
{
  m_global_vis_parameters = visparams;
  m_datacontainer = datacontainer;
}

Histogram::~Histogram()
{
}

QString Histogram::createJSONString(int mito_id)
{
  Object* mito = m_datacontainer->getObject(mito_id);

  std::vector<int>* mito_indices = mito->get_indices_list();
  std::vector<VertexData>* vertices = m_datacontainer->getMesh()->getVerticesList();

  QJsonArray json;
  for (auto i : *mito_indices)
  {
    VertexData vertex = vertices->at(i);
    double distance_to_cell = vertex.distance_to_cell;
    
    QJsonObject distance;
    distance.insert("dist", QJsonValue::fromVariant(distance_to_cell));
    json.push_back(distance);
  }

  QJsonDocument doc(json);
  return doc.toJson(QJsonDocument::Compact);
}

QWebEngineView* Histogram::initVisWidget(int ID, SpecificVisParameters params)
{
  QString json = createJSONString(ID);
  m_data = new HistogramData(json, m_datacontainer, m_global_vis_parameters);

  setSpecificVisParameters(params);

  m_web_engine_view = new QWebEngineView();
  QWebChannel* channel = new QWebChannel(m_web_engine_view->page());
  m_web_engine_view->page()->setWebChannel(channel);
  channel->registerObject(QStringLiteral("histogram_data"), m_data);
  m_web_engine_view->load(getHTMLPath(m_index_filename));

  return m_web_engine_view;
}

QWebEngineView* Histogram::getWebEngineView()
{
  return m_web_engine_view;
}

bool Histogram::update()
{
  return false;
}

Histogram* Histogram::clone()
{
  return new Histogram(this);
}

VisType Histogram::getType()
{
  return VisType::HISTOGRAM;
}

void Histogram::setSpecificVisParameters(SpecificVisParameters params)
{
  m_data->setNumberOfBins(params.number_of_bins);
  m_data->setColors(params.colors);
}

HistogramData::HistogramData(QString json_string, DataContainer* datacontainer, GlobalVisParameters* visparameters)
{
  m_json_string = json_string;
  m_datacontainer = datacontainer;
  visparameters = visparameters;
}

HistogramData::~HistogramData()
{
}

Q_INVOKABLE QString HistogramData::getData()
{
  return Q_INVOKABLE m_json_string;
}

Q_INVOKABLE int HistogramData::getNumberOfBins()
{
  return Q_INVOKABLE m_number_of_bins;
}

Q_INVOKABLE QString HistogramData::getColormap()
{
  return Q_INVOKABLE m_colors;
}
