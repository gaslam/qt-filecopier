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
    void initTestCase(){
#ifdef SOURCE_FILE
        m_Source.setFileName(SOURCE_FILE);
        QString sourceNotExistingMsg{"Source does not exist or path is invalid: "};
        sourceNotExistingMsg += SOURCE_FILE;
        QVERIFY2(m_Source.exists(),sourceNotExistingMsg.toStdString().c_str());


        QString sourceNotOpenedMsg{"Source cannot be opened: "};
        sourceNotOpenedMsg += SOURCE_FILE;
        QVERIFY2(m_Source.open(QIODevice::ReadOnly),sourceNotOpenedMsg.toStdString().c_str());

        m_Source.close();
#elif
        QFAIL("source is not provided");
#endif

#ifdef DESTINATION_DIR
        m_Destination.setFileName(DESTINATION_DIR);

        QFileInfo destinationInfo{m_Destination};
        QString destNotOpenedMsg{"Destination cannot be opened: "};
        sourceNotOpenedMsg += DESTINATION_DIR;
        QVERIFY2(destinationInfo.dir().exists(),destNotOpenedMsg.toStdString().c_str());

        QString destNotFileMsg{"Destination is not a file: "};
        destNotFileMsg += DESTINATION_DIR;
        QVERIFY2(!destinationInfo.isFile(),destNotOpenedMsg.toStdString().c_str());
#elif
        QFAIL("destination is not provided");
#endif
    }
private:
    QFile m_Source;
    QFile m_Destination;
};

#endif // COPIERBENCHMARKS_H
