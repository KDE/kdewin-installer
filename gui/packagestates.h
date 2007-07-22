#ifndef PACKAGESTATES_H
#define PACKAGESTATES_H


#include <QString>
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

class PackageStates {
  public:
    PackageStates() {}
    void setState(QString pkgName, QString pkgVersion, Package::Type type, stateType state);
    stateType getState(QString pkgName, QString pkgVersion, Package::Type type  );

  protected:
    QHash<QString,PackageFlags> states; 
};

#endif
