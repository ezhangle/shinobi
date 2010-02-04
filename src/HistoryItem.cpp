#include "HistoryItem.h"

namespace shinobi {
  int HistoryItem::registerInQtTypeSystem() {
    int typeId = qRegisterMetaType<HistoryItem>("HistoryItem");
    qRegisterMetaTypeStreamOperators<HistoryItem>(QMetaType::typeName(typeId));
    qRegisterMetaTypeStreamOperators<HistoryItem>(QMetaType::typeName(qMetaTypeId<HistoryItem>()));

    return typeId;
  }

  int HistoryItem::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi
