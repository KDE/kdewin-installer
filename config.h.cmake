#ifndef CONFIG_H
#define CONFIG_H
#cmakedefine VERSION "@VERSION@"
#cmakedefine CMAKE_SOURCE_DIR "@CMAKE_SOURCE_DIR@"
#cmakedefine SEVENZIP_UNPACK_SUPPORT 1

#include <QString>
#include <QtDebug>
inline QString &DUMP_INDENT(int inc = 0)
{
    static int dumpLevel = 0;
    static QString l="";
    dumpLevel += inc;
    return l.fill(' ',dumpLevel);
}

inline void DUMP_HEADER(const QString &title, const QString &_class=QString())
{
//    qDebug() << "-------------" << title << _class << "---(Start)------";
    qDebug() << DUMP_INDENT(1) << title << _class ;
}    
    
inline void DUMP_FOOTER(const QString &title, const QString &_class=QString())
{
//    qDebug() << "-------------" << title << _class << "---(End)--------";
      qDebug() << DUMP_INDENT(-1);
}

#endif
