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


void QtCopyCopier::run()
{
    QFileInfo destination{Destination()};
    QFileInfo source{Source()};
    QFile::copy(source.absoluteFilePath(),destination.absoluteFilePath());
}


void MappedCopier::run()
{
    //Get source and destination files for QFile
    QFileInfo destination{Destination()};
    QFileInfo source{Source()};
    QFile sourceFile{source.absoluteFilePath()};
    QFile destinationFile{destination.absoluteFilePath()};

    if(!sourceFile.open(QIODevice::ReadOnly))
    {
        qCritical() << sourceFile.errorString();
        return;
    }

    if(!destinationFile.open(QIODevice::ReadWrite)) {
        qCritical() << destinationFile.errorString();
        return;
    }

    const qint64 sourceSize{source.size()};

    //Resize the file. Otherwise errors are thrown.
    destinationFile.resize(sourceSize);

    int actualChunkSize{};

    //Indicates the current position of where the bytes should be written
    //It's always at the end of the written bytes to the file
    qint64 pos{};

    while(pos < sourceSize)
    {
        //Determine the size that needs to be written.
        //if the remaining bytes left are smaller in size than m_Chunksize, subtract pos from source size
        //Otherwise, choose the chunksize
        actualChunkSize = std::min(m_ChunkSize, sourceSize - pos);

        //Map source and destination.
        //The bytes come from the chunksize starting from the current position
        uchar* src{sourceFile.map(pos,actualChunkSize)};
        uchar* dest {destinationFile.map(pos, actualChunkSize)};

        //Copy the chunk to the destination
        memcpy(dest,src,actualChunkSize);

        //Release the memory
        sourceFile.unmap(src);
        destinationFile.unmap(dest);

        pos += actualChunkSize;
    }
}
