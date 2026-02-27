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
    QFileInfo destination{Destination()};
    QFileInfo source{Source()};
    QFile sourceFile{source.absoluteFilePath()};
    QFile destinationFile{destination.absoluteFilePath()};
    qint64 pos{};

    if(!sourceFile.open(QIODevice::ReadOnly))
    {
        qCritical() << sourceFile.errorString();
        return;
    }

    if(!destinationFile.open(QIODevice::ReadWrite)) {
        qCritical() << destinationFile.errorString();
        return;
    }

    destinationFile.resize(source.size());

    int actualChunkSize{};

    while(pos < sourceFile.size())
    {
actualChunkSize = std::min(m_ChunkSize, source.size() - pos);
        uchar* src{sourceFile.map(pos,actualChunkSize)};

        uchar* dest {destinationFile.map(pos, actualChunkSize)};

        memcpy(dest,src,actualChunkSize);
        sourceFile.unmap(src);
        destinationFile.unmap(dest);
        pos += actualChunkSize;
    }
}
