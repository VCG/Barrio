#include <qwebengineview.h>

enum InfoVisType
{
  BAR_CHART,
  SKELETON_CHART,
  DISTANCE_TREE,
  CONNECTION_MATRIX
};

class InfoVisWidget: public QWebEngineView
{
public:
  InfoVisWidget(QUrl url);
};

