#ifndef PACKAGESTATES_H
#define PACKAGESTATES_H

#include <QString>
#include <QDebug>
#include "package.h"
#include "packagelist.h"
#include "typehelper.h"

class Database;

enum stateType { _Nothing = 0, _Install = 1, _Update = 2, _Remove = 4}; // make sure _Install != 0 !!
Q_DECLARE_FLAGS(stateTypes,stateType);
Q_DECLARE_OPERATORS_FOR_FLAGS(stateTypes);

/** 
 The PackageStates class contains the state set from the gui for each package 
*/

class PackageFlags {
  public: 
    PackageFlags() 
    { 
        all = bin = lib  = doc = src = dbg = _Nothing; 
    };
    stateType all,bin,lib,doc,src,dbg; 
};

typedef QHash<QString,PackageFlags> PackageStatesType;

class PackageStates {
  public:
    PackageStates() {}
    void setState(const QString &pkgName, const QString &pkgVersion, FileTypes::Type type, stateType state);
    void setState(const Package *pkg, FileTypes::Type type, stateType state);
    stateType getState(const QString &pkgName, const QString &pkgVersion, FileTypes::Type type  );
    stateType getState(const Package *pkg, FileTypes::Type type);
    QList <Package *>packages(PackageList *list);
    QList <Package *>packages(Database *database);
    void clear();
    friend QDebug operator<<(QDebug, const PackageStates &);

  protected:
    PackageStatesType m_states; 
    inline const QString &getKey(const QString &pkgName, const QString &pkgVersion) { return pkgName; }
};

#endif
