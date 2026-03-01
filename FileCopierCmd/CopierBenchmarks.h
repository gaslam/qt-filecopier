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
    //Check if the source exists, before tests can start.
    //Verify that the source exists and is readable.
    //If this fails, the benchmark results would be meaningless.
    void checkIfSourceExists(){

        const QString sourceFile{getSourceFile()};
        QFile source{};
        source.setFileName(sourceFile);
        QString sourceNotExistingMsg{"Source does not exist or path is invalid: "};
        sourceNotExistingMsg += sourceFile;
        QVERIFY2(source.exists(),sourceNotExistingMsg.toStdString().c_str());


        QString sourceNotOpenedMsg{"Source cannot be opened: "};
        sourceNotOpenedMsg += sourceFile;
        QVERIFY2(source.open(QIODevice::ReadOnly),sourceNotOpenedMsg.toStdString().c_str());
    }

    //Checks if the destination exists, before the tests start
    void checkIfDestinationExists()
    {
        const QString destinationDir{getDestinationDir()};
        QFile dest{};

        dest.setFileName(destinationDir);

        QFileInfo destinationInfo{dest};
        QString destNotOpenedMsg{"Destination cannot be opened: "};
        destNotOpenedMsg += destinationDir;
        QVERIFY2(destinationInfo.dir().exists(),destNotOpenedMsg.toStdString().c_str());
    }

    //Test process with QFile::copy
    void copyWithQFileCopy()
    {
        QFileInfo sourceFile{getSourceFile()};

        process<QtCopyCopier>(sourceFile);
    }

    //Test processThreaded with QFile::copy
    void copyWithQFileThreaded()
    {
        QFileInfo sourceFile{getSourceFile()};

        processThreaded<QtCopyCopier>(sourceFile,10);
    }

    //Test process with a memory mapped file
    void copyWithMappedCopy()
    {
        QFileInfo sourceFile{getSourceFile()};

        process<MappedCopier>(sourceFile);
    }

    //Test processThreaded with memory mapped copies
    void copyWithMappedCopyThreaded()
    {
        QFileInfo sourceFile{getSourceFile()};

        processThreaded<MappedCopier>(sourceFile,10);
    }

private:

    //Processes a predetermined amount of files with QtConcurrent
    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void processThreaded(QFileInfo& fileSource, size_t files = 100)
    {
        qInfo() << "Benchmarking " + QString::number(files) + " files threaded!!";

        //Get the destination dir and make a full string with the target file
        const QString destinationDir{getDestinationDir()};

        //Initialize the copiers and set there source and destination files.
        QList<T> fileCopiers{};

        fileCopiers.reserve(files);

        for(size_t i{}; i < files; ++i)
        {
            T fileCopier{};
            //TODO rewrite this with QString args funtion for string interpolation.
            const QString dest{QString(destinationDir)+  fileSource.fileName() + "_copy" + QString::number(i) + "." + fileSource.suffix()};

            fileCopier.setSource(fileSource.absoluteFilePath());
            fileCopier.setDestination(dest);
            fileCopiers << fileCopier;
        }

        //TODO allow single threads for HDD
        QThreadPool pool;
        pool.setMaxThreadCount(4);
        //Start the process
        QBENCHMARK_ONCE{
            //Allocates the threads and runs them
            //I already chose a future to allow for threads to be paused
            QFuture<void> future = QtConcurrent::map(&pool,fileCopiers,[](T& copier){
                copier.run();
            });
            //wait for the results to be finished
            future.waitForFinished();
        }

        //Delete all destinations and if check if they exist.
        foreach(auto& copier, fileCopiers)
        {
            const QFileInfo& currDestination{copier.Destination()};
            QVERIFY2(currDestination.exists(),"Destination folder does not exist anymore!!");
            QFile::remove(currDestination.absoluteFilePath());
        }
        return;
    }

    //Processes a single copy of the source file
    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void process(QFileInfo& fileSource)
    {
        qInfo() << "Benchmarking file!!";

        //Get the destination dir and make a full string with the target file
        const QString destinationDir{getDestinationDir()};
        const QString dest{QString(destinationDir)+ "_copy_" +  fileSource.fileName()};

        //Copier initialization
        T fileCopier{};
        fileCopier.setSource(fileSource.absoluteFilePath());
        fileCopier.setDestination(dest);
        //Start the process
        //Not doing this once will result in inacurrate speeds
        //Is there anything to fix this?
        QBENCHMARK_ONCE{
            fileCopier.run();
        }


        //Check if the destination exists before removal
        const auto& destination{fileCopier.Destination()};
        QVERIFY2(destination.exists(),"Destination folder does not exists anymore!!");


        QFile::remove(destination.absoluteFilePath());
        return;
    }

    //I could not provide any arguments through the QBENCHMARK console app
    //So I am doing this with CMAKE variables for now
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
};

#endif // COPIERBENCHMARKS_H
