#ifndef FILECOPIER_H
#define FILECOPIER_H

#include <QFile>
#include <QFileInfo>
#include <QFuture>


class FileCopier
{
public:
    FileCopier();
    virtual ~FileCopier() = default;

    // QRunnable interface
public:
    virtual void run() = 0;

    void setSource(const QFileInfo &newSource);
    void setSource(const QString& newSource);

    [[nodiscard]] QFileInfo Destination() const;
    void setDestination(const QFileInfo &newDestination);
    void setDestination(const QString &newDestination);

    [[nodiscard]] QFileInfo Source() const;

private:
    QFileInfo m_Source{};
    QFileInfo m_Destination{};
};

class QtCopyCopier: public FileCopier
{
public:

    void run() override;
};

class MappedCopier: public FileCopier
{
public:

    void run() override;

private:
    qint64 m_ChunkSize{8*1024*1024};
};


#endif // FILECOPIER_H
