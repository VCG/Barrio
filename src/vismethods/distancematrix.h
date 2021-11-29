#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include "vismethod.h"

#include "../datacontainer.h"
#include "../globalParameters.h"

class DistanceMatrixData : public QObject
{
	Q_OBJECT

public:
	DistanceMatrixData(QString json_string, DataContainer* datacontainer, GlobalVisParameters* visparameters);
	~DistanceMatrixData();

	Q_INVOKABLE QString getData();
	Q_PROPERTY(QString json_string READ getData);

	void setJsonString(QString json) { m_json_string = json; }
	QString getJsonString() { return m_json_string; }

	Q_INVOKABLE void setHighlightedStructure(const QString& name);
	Q_INVOKABLE void removeHighlightedStructure(const QString& name_to_remove);

	Q_INVOKABLE void setHighlightedFrame(const QString& name);
	Q_INVOKABLE void removeHighlightedFrame(const QString& name_to_remove);

	QString m_json_string;

	GlobalVisParameters* m_global_vis_parameters;
	DataContainer* m_datacontainer;

};

class DistanceMatrix : public IVisMethod
{
public:
	DistanceMatrix(DistanceMatrix* matrix);
	DistanceMatrix(GlobalVisParameters* visparams, DataContainer* datacontainer);
	~DistanceMatrix();

	QWebEngineView* initVisWidget(int ID, SpecificVisParameters params);
	bool            update();
	QWebEngineView* getWebEngineView();
	DistanceMatrix* clone();
	bool            update_needed();

	VisType         getType();

	void            setSpecificVisParameters(SpecificVisParameters params);

private:
	DistanceMatrixData* data;
	QJsonObject m_settings;

	DataContainer* m_datacontainer;
	QString m_title;

	QString m_index_filename = "web/distancematrix_index.html";
	QWebEngineView* m_web_engine_view;

	GlobalVisParameters* m_global_vis_parameters;
	QString getJSONString(QList<int>* selected_mitos, double distanceThreshold);
};

#endif
