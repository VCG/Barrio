#include "vismethod.h"

class Barchart : public IVisMethod 
{
public:
  Barchart();
  ~Barchart();

  InfoVisWidget* getVisWidget();
  QString getName();

private:
  QList<QString>* m_labels;
  QList<float>* m_values;

  QString m_x_title;
  QString m_y_title;
  QString m_title;
};
