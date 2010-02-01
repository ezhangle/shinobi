#ifndef __SHINOBI_DRIVE_FILTER_H__
#define __SHINOBI_DRIVE_FILTER_H__

#include "config.h"
#include <QList>
#include <QRegExp>
#include <QDataStream>
#include <QMetaType>

namespace shinobi {
// -------------------------------------------------------------------------- //
// DriveFilter
// -------------------------------------------------------------------------- //
  class DriveFilter {
  public:
    DriveFilter() {}

    DriveFilter(const QString& pathPattern, const QString& namePattern, const QString& serialNoPattern): 
      mPathRegExp(pathPattern, Qt::CaseInsensitive, QRegExp::Wildcard),
      mNameRegExp(namePattern, Qt::CaseInsensitive, QRegExp::Wildcard), 
      mSerialNoRegExp(serialNoPattern, Qt::CaseInsensitive, QRegExp::Wildcard) {}
    
    bool matches(const QString& drivePath, const QString& driveName, const QString& driveSerialNo) const {
      return mPathRegExp.exactMatch(drivePath) && mNameRegExp.exactMatch(driveName) && mSerialNoRegExp.exactMatch(driveSerialNo);
    }

    QString pathPattern() const {
      return mPathRegExp.pattern();
    }

    QString namePattern() const {
      return mNameRegExp.pattern();
    }

    QString serialNoPattern() const {
      return mSerialNoRegExp.pattern();
    }

    friend QDataStream &operator<<(QDataStream &out, const DriveFilter &filter) {
      return out << filter.mPathRegExp.pattern() << filter.mNameRegExp.pattern() << filter.mSerialNoRegExp.pattern();
    }

    friend QDataStream &operator>>(QDataStream &in, DriveFilter &filter) {
      QString pathPattern, namePattern, serialNoPattern;
      in >> pathPattern >> namePattern >> serialNoPattern;
      filter.~DriveFilter();
      new (&filter) DriveFilter(pathPattern, namePattern, serialNoPattern);
      return in;
    }

  private:
    static int registerInQtTypeSystem();

    QRegExp mPathRegExp, mNameRegExp, mSerialNoRegExp;
    static int sQtMetaTypeId;
  };

}

Q_DECLARE_METATYPE(::shinobi::DriveFilter);
Q_DECLARE_METATYPE(QList<::shinobi::DriveFilter>);

#endif // __SHINOBI_DRIVE_FILTER_H__
