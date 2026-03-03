#include "filecopier.h"

#include <QFile>

FileCopier::FileCopier() {}

QFileInfo FileCopier::Source() const
{
    return m_Source;
}

void FileCopier::setSource(const QFileInfo &newSource)
{
    m_Source = newSource;
}

void FileCopier::setSource(const QString &newSource)
{
    m_Source = QFileInfo{newSource};
}

QFileInfo FileCopier::Destination() const
{
    return m_Destination;
}

void FileCopier::setDestination(const QFileInfo &newDestination)
{
    m_Destination = newDestination;
}

void FileCopier::setDestination(const QString& newDestination)
{
    m_Destination = QFileInfo(newDestination);
}


bool QtCopyCopier::run()
{
    QFileInfo destination{Destination()};
    QFileInfo source{Source()};
    return QFile::copy(source.absoluteFilePath(),destination.absoluteFilePath());
}


bool MappedCopier::run()
{
    //Get source and destination files for QFile
    QFileInfo destination{Destination()};
    QFileInfo source{Source()};
    QFile sourceFile{source.absoluteFilePath()};
    QFile destinationFile{destination.absoluteFilePath()};

    if(!sourceFile.open(QIODevice::ReadOnly))
    {
        qCritical() << sourceFile.errorString();
        return false;
    }

    if(!destinationFile.open(QIODevice::ReadWrite)) {
        qCritical() << destinationFile.errorString();
        return false;
    }

    const qint64 sourceSize{source.size()};

    //Resize the file. Otherwise errors are thrown.
    destinationFile.resize(sourceSize);

    QByteArray buffer{};
    uchar* dest {destinationFile.map(0, sourceSize)};
    int dataRead{};
    do
    {
        //Read the chunk from the source file and add it to the destination.
        //Add the total amount of bytes read to dataRead.
        dataRead = sourceFile.read(reinterpret_cast<char*>(dest),m_ChunkSize);
        dest += dataRead;
    }while(dataRead > 0);

    destinationFile.unmap(dest);
    return destinationFile.flush();

}
