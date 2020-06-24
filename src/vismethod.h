#include <QString>
#include "infoviswidget.h"

class IVisMethod
{
public:
  IVisMethod();
  virtual ~IVisMethod();
  virtual QString getName() = 0;
  virtual InfoVisWidget* getVisWidget() = 0;

protected:
  QString m_name;
};

