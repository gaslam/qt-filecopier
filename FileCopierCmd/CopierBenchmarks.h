#ifndef COPIERBENCHMARKS_H
#define COPIERBENCHMARKS_H
#include "filecopier.h"
#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QTest>
#include <qfuture.h>
#include <qtestcase.h>
#include <QtConcurrent>

class CopierBenchmarks : public QObject
{
    Q_OBJECT
private slots:
    void checkIfSourceExists(){

        const QString sourceFile{getSourceFile()};
        m_Source.setFileName(sourceFile);
        QString sourceNotExistingMsg{"Source does not exist or path is invalid: "};
        sourceNotExistingMsg += sourceFile;
        QVERIFY2(m_Source.exists(),sourceNotExistingMsg.toStdString().c_str());


        QString sourceNotOpenedMsg{"Source cannot be opened: "};
        sourceNotOpenedMsg += sourceFile;
        QVERIFY2(m_Source.open(QIODevice::ReadOnly),sourceNotOpenedMsg.toStdString().c_str());
    }

    void checkIfDestinationExists()
    {
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


    void copyWithQFileThreaded()
    {
        QFileInfo sourceFile{getSourceFile()};

        processThreaded<QtCopyCopier>(sourceFile,100);
    }

    void copyWithMappedCopy()
    {
        QFileInfo sourceFile{getSourceFile()};

        process<MappedCopier>(sourceFile);
    }

    void copyWithMappedCopyThreaded()
    {
        QFileInfo sourceFile{getSourceFile()};

        processThreaded<MappedCopier>(sourceFile,100);
    }

private:

    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void processThreaded(QFileInfo& fileSource, size_t files = 100)
    {
        qInfo() << "Benchmarking " + QString::number(files) + " files threaded!!";
        const QString destinationDir{getDestinationDir()};
        const QString dest{QString(destinationDir)+  fileSource.fileName() + "_copy" + "." + fileSource.suffix()};
        QList<T> fileCopiers{};
        fileCopiers.reserve(files);

        for(size_t i{}; i < files; ++i)
        {
            T fileCopier{};
            const QString dest{QString(destinationDir)+  fileSource.fileName() + "_copy" + QString::number(i) + "." + fileSource.suffix()};

            fileCopier.setSource(fileSource.absoluteFilePath());
            fileCopier.setDestination(dest);
            fileCopiers << fileCopier;
        }

        QBENCHMARK_ONCE{
            QFuture<void> future = QtConcurrent::map(fileCopiers,[](T& copier){
                copier.run();
            });
            future.waitForFinished();
        }

        QVERIFY2(fileSource.exists(),"Source file does not exists anymore!!");

        foreach(auto& copier, fileCopiers)
        {
            const QFileInfo& currDestination{copier.Destination()};
            QVERIFY2(currDestination.exists(),"Destination folder does not exist anymore!!");
            QFile::remove(currDestination.absoluteFilePath());
        }
        return;
    }

    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void process(QFileInfo& fileSource)
    {
        qInfo() << "Benchmarking file!!";
        const QString destinationDir{getDestinationDir()};
        const QString dest{QString(destinationDir)+ "_copy_" +  fileSource.fileName()};
        QString uniqueDest = dest + QString::number(QRandomGenerator::global()->generate());

        T fileCopier{};
        fileCopier.setSource(fileSource.absoluteFilePath());
        fileCopier.setDestination(uniqueDest);

        QBENCHMARK_ONCE{
            fileCopier.run();
        }

        const auto& source{fileCopier.Source()};
        QVERIFY2(source.exists(),"Source file does not exists anymore!!");

        const auto& destination{fileCopier.Destination()};
        QVERIFY2(destination.exists(),"Destination folder does not exists anymore!!");


        QFile::remove(fileCopier.Destination().absoluteFilePath());
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
