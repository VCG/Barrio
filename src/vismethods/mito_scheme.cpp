#include "mito_scheme.h"

Q_INVOKABLE QString MitoSchemeData::getData()
{
  return Q_INVOKABLE m_json_string;
}