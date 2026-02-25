#ifndef COPIERBENCHMARKS_H
#define COPIERBENCHMARKS_H
#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QBenchmark.h>
#include <QTest>
#include <qtestcase.h>

class CopierBenchmarks : public QObject
{
    Q_OBJECT
private slots:
private:
    QFile m_Source;
    QFile m_Destination;
};

#endif // COPIERBENCHMARKS_H
