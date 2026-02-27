#include "filecopier.h"

#include <QFile>

FileCopier::FileCopier() {}

QString FileCopier::Source() const
{
    return m_Source;
}

void FileCopier::setSource(const QString &newSource)
{
    m_Source = newSource;
}

QString FileCopier::Destination() const
{
    return m_Destination;
}

void FileCopier::setDestination(const QString &newDestination)
{
    m_Destination = newDestination;
}


void QtCopyCopier::run(QFuture<void> &future)
{
    QString destination{Destination()};
    QString source{Source()};
    QFile::copy(source,destination);
}

