#ifndef CONFIG_H
#define CONFIG_H
#cmakedefine VERSION "@VERSION@"
#cmakedefine CMAKE_SOURCE_DIR "@CMAKE_SOURCE_DIR@"
#cmakedefine SEVENZIP_UNPACK_SUPPORT 1

#include <QString>
#include <QtDebug>

inline void DUMP_HEADER(const QString &title, const QString &_class=QString())
{
    qDebug() << "-------------" << title << _class << "---(Start)------";
}    
    
inline void DUMP_FOOTER(const QString &title, const QString &_class=QString())
{
    qDebug() << "-------------" << title << _class << "---(End)--------";
}

#endif
