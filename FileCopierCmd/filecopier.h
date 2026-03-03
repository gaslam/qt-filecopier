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
public:
    virtual bool run() = 0;

    void setSource(const QFileInfo &newSource);
    void setSource(const QString& newSource);

    [[nodiscard]] QFileInfo Destination() const;
    void setDestination(const QFileInfo &newDestination);
    void setDestination(const QString &newDestination);

    [[nodiscard]] QFileInfo Source() const;

private:
    //TODO, just makes this into strings with file paths
    QFileInfo m_Source{};
    QFileInfo m_Destination{};
};

class QtCopyCopier: public FileCopier
{
public:

    bool run() override;
};

class MappedCopier: public FileCopier
{
public:

    bool run() override;

private:
    //8 MB chunk size
    static constexpr qint64 m_ChunkSize{8388608};
};


#endif // FILECOPIER_H
