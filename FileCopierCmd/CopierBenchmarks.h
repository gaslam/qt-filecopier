#ifndef COPIERBENCHMARKS_H
#define COPIERBENCHMARKS_H
#include "filecopier.h"
#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QBenchmark.h>
#include <QTest>
#include <qfuture.h>
#include <qtestcase.h>
#include <QtConcurrent>

class CopierBenchmarks : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase(){

        const QString sourceFile{getSourceFile()};
        m_Source.setFileName(sourceFile);
        QString sourceNotExistingMsg{"Source does not exist or path is invalid: "};
        sourceNotExistingMsg += sourceFile;
        QVERIFY2(m_Source.exists(),sourceNotExistingMsg.toStdString().c_str());


        QString sourceNotOpenedMsg{"Source cannot be opened: "};
        sourceNotOpenedMsg += sourceFile;
        QVERIFY2(m_Source.open(QIODevice::ReadOnly),sourceNotOpenedMsg.toStdString().c_str());

        m_Source.close();

        const QString destinationDir{getDestinationDir()};

        m_Destination.setFileName(destinationDir);

        QFileInfo destinationInfo{m_Destination};
        QString destNotOpenedMsg{"Destination cannot be opened: "};
        destNotOpenedMsg += destinationDir;
        QVERIFY2(destinationInfo.dir().exists(),destNotOpenedMsg.toStdString().c_str());

    }

    void copyWithQFileCopy()
    {
        QFileInfo sourceFile{getSourceFile()};

        process<QtCopyCopier>(sourceFile);
    }

private:

    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void process(QFileInfo& fileSource)
    {
        qInfo() << "Benchmarking file!!";
        const QString destinationDir{getDestinationDir()};
        const QString dest{QString(destinationDir)+  fileSource.fileName() + "_copy" + "." + fileSource.suffix()};

        T fileCopier{};
        QFuture<void> future = QtConcurrent::run([&fileCopier,&future](){
            fileCopier.run(future);
        });
        fileCopier.setSource(fileSource.absoluteFilePath());
        fileCopier.setDestination(dest);

        QBENCHMARK{
            {

                future.waitForFinished();
            }
        }

        QFile::remove(dest);
        return;
    }

    inline QString getSourceFile(){
#ifdef SOURCE_FILE
        return QString{SOURCE_FILE};
#endif
        return "";
    }

    [[nodiscard]] inline QString getDestinationDir()
    {
#ifdef DESTINATION_DIR
        return QString(DESTINATION_DIR);
#else
        return QString();
#endif
    }

    QFile m_Source;
    QFile m_Destination;
};

#endif // COPIERBENCHMARKS_H
