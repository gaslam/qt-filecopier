#ifndef FILECOPIER_H
#define FILECOPIER_H

#include <QFile>
#include <QFuture>


class FileCopier
{
public:
    FileCopier();
    virtual ~FileCopier() = default;

    // QRunnable interface
public:
    virtual void run(QFuture<void>& future) = 0;

    void setSource(const QString &newSource);

    [[nodiscard]] QString Destination() const;
    void setDestination(const QString &newDestination);

    [[nodiscard]] QString Source() const;

private:
    QString m_Source{};
    QString m_Destination{};
};

class QtCopyCopier: public FileCopier
{
public:

    void run(QFuture<void>& future) override;
};


#endif // FILECOPIER_H
