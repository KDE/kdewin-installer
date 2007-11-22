#ifndef PACKAGESTATES_H
#define PACKAGESTATES_H


#include <QString>
#include <QDebug>
#include "package.h"
#include "packagelist.h"

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
        all = bin = lib  = doc = src = _Nothing; 
    };
    stateType all,bin,lib,doc,src; 
};

typedef QHash<QString,PackageFlags> PackageStatesType;

class PackageStates {
  public:
    PackageStates() {}
    void setState(QString pkgName, QString pkgVersion, Package::Type type, stateType state);
    void setState(Package *pkg, Package::Type type, stateType state);
    stateType getState(QString pkgName, QString pkgVersion, Package::Type type  );
    stateType getState(Package *pkg, Package::Type type);
    QList <Package *>packages(PackageList *list);
    void clear();

  protected:
    PackageStatesType m_states; 
    friend QDebug &operator<<(QDebug &, const PackageStates &);
    QString getKey(QString pkgName, QString pkgVersion) { return pkgName; }
};

QDebug &operator<<(QDebug &, const PackageStates &);
#endif
