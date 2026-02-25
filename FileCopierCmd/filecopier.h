#ifndef FILECOPIER_H
#define FILECOPIER_H

#include <QFile>
#include <QRunnable>


class FileCopier : public QRunnable
{
public:
    FileCopier();

    // QRunnable interface
public:
    void run() override;

private slots:
};

#endif // FILECOPIER_H
