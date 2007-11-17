#ifndef PACKAGESTATES_H
#define PACKAGESTATES_H


#include <QString>
#include <QDebug>
#include "package.h"

enum stateType { _Nothing, _Install, _Update, _Remove}; // make sure _Install != 0 !!

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
    stateType getState(QString pkgName, QString pkgVersion, Package::Type type  );
    void clear();

  protected:
    PackageStatesType m_states; 
    friend QDebug &operator<<(QDebug &, const PackageStates &);
};

QDebug &operator<<(QDebug &, const PackageStates &);
#endif
