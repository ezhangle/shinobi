#include "HistoryItem.h"

namespace shinobi {
  int HistoryItem::registerInQtTypeSystem() {
    int typeId = qRegisterMetaType<HistoryItem>();
    qRegisterMetaTypeStreamOperators<HistoryItem>(QMetaType::typeName(typeId));

    return typeId;
  }

  int HistoryItem::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi
