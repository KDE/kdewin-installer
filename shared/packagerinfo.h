#ifndef PACKAGER_INFO_H
#define PACKAGER_INFO_H

#include <QFile>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QRegExp>

class FileSizeInfo
{
public:
    int installedSize; 
    int compressedSize; 
};

class PackagerInfo
{
public:
    bool writeToFile(const QString &file)
    {            
        QFile f(file);
        if (!f.open(QIODevice::WriteOnly))
            return false;
        QString line;
        line += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        line += "<package name=\"" + name + "\" type=\"runtime\">\n";
        line += "  <version>" + version + "</version>\n";
        line += "  <compiler>" + compilerType + "</compiler>\n";
        line += "  <checksum type=\"" + hash.typeAsString() + "\">" + hash.value().toHex() + "</checksum>\n";
        line += "  <size type=\"installed\">" + QString::number(size.installedSize) + "</size>\n";
        line += "  <size type=\"compressed\">" + QString::number(size.compressedSize) + "</size>\n";
        line += "  <dependencies>" + dependencies.join(" ") + "</dependencies>\n";
        line += "</package>\n";
        f.write(line.toUtf8());
        f.close();
        return true;
    }

    bool readFromFile(const QString &file)
    {            
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly))
            return false;
        while (!f.atEnd()) {
            QByteArray line = f.readLine();
            line.replace("\n","");
            line = line.trimmed();
            
            if (line.contains("<package") )
            {
                //"<package name=\"" + name + "\" type=\"runtime\">\n";
                // only runtime package supported yet
                if (!line.contains("runtime"))
                    return false;
                line.replace("<package name=\"","");
                line.replace("\" type=\"runtime\">","");
                name = line;
            }
            else if (line.contains("<version>"))
            {
                line.replace("<version>","");
                line.replace("</version>","");
                version = line;
            }
            else if (line.contains("<compiler>"))
            {
                //"  <compiler>" + compilerType + "</version>\n";
                ;
            }
            else if (line.contains("<checksum"))
            {
                //"  <checksum type=\"" + hash.typeAsString() + "\">" + hash.value().toHex() + "</checksum>\n";
                ;
            }
            else if (line.contains("<size type=\"installed\""))
            {
                //"  <size type=\"installed\">" + QString::number(size.installedSize) + "</size>\n";
                ;
            }
            else if (line.contains("<size type=\"compressed\""))
            {
                //"  <size type=\"compressed\">" + QString::number(size.compressedSize) + "</size>\n";
                ;
            }
            else if (line.contains("<dependencies>"))
            {
                line.replace("<dependencies>","");
                line.replace("</dependencies>","");
                QString a = line;
                dependencies = a.split(' ');
            }
        }
        f.close();
        return true;
    }

    FileSizeInfo size;
    HashValue hash;
	QString version;
	QString name;
	QString compilerType;
    QStringList dependencies; 
}; 

#endif