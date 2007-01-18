
#include "qua7zip.h"

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFile>

#ifndef QUNZIP_BUFFER
# define QUNZIP_BUFFER (256 * 1024)
#endif
using namespace qua7zip;

bool unzipFile(const QString &destpath, const QString &zipFile)
{
    QDir path(destpath);
    Qua7zip manager( "C:/tmp/cpp7zip-build/test_1.7z" );
	Qua7zipFileInfo info;
    Qua7zipFile file(&manager);

    if( manager.open(Qua7zip::mdUnpack) )
    {
        return false;
    }

    if(!path.exists())
    {
        return false;
    }

    for(bool bOk = manager.goToFirstFile(); bOk; bOk = manager.goToNextFile())
    {
        // get file informations
        if(!manager.getCurrentFileInfo(info))
        {
            return false;
        }
        QFileInfo fi(path.filePath(info.fileName));

        // is it's a subdir ?
		if( info.isDir )
        {
            if(fi.exists())
            {
                if(!fi.isDir())
                {
                    return false;
                }
                continue;
            }
            if(!path.mkdir(fi.absoluteFilePath()))
            {
                return false;
            }
            continue;
        }
        if(!path.exists(fi.absolutePath()))
        {
            if (!path.mkpath(fi.absolutePath()))
            {
                return false;
            }
        }
        // create new file
        QFile newFile(fi.absoluteFilePath());
        if( !newFile.open( QIODevice::WriteOnly ) )
        {
            return false;
        }

         // open file
        if(!file.open(QIODevice::ReadOnly))
        {
            return false;
        }

        // copy data
        qint64 iBytesRead;
        QByteArray ba;
        ba.resize(QUNZIP_BUFFER);

        while((iBytesRead = file.read(ba.data(), QUNZIP_BUFFER)) > 0)
            newFile.write(ba.data(), iBytesRead);

        file.close();
        newFile.close();

		if( manager.status() != SZ_OK )
        {
            return false;
        }
    }
    manager.close();

    return true;
}

void main()
{
	unzipFile( QDir::currentPath(),"test_1.7z");
}
