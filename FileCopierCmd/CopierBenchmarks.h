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

public:
    CopierBenchmarks(size_t iterations, const QString& source, const QString& destination) :
        m_Iterations{iterations},
        m_SourceFile{source},
        m_DestDir{destination}
    {}
private slots:

    //Check if the source exists, before tests can start.
    //Verify that the source exists and is readable.
    //If this fails, the benchmark results would be meaningless.
    void checkIfSourceExists(){

        const QString sourceFile{m_SourceFile};
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
        QFile dest{};

        dest.setFileName(m_DestDir);

        QFileInfo destinationInfo{dest};
        QString destNotOpenedMsg{"Destination cannot be opened: "};
        destNotOpenedMsg += m_DestDir;
        QVERIFY2(destinationInfo.dir().exists(),destNotOpenedMsg.toStdString().c_str());
    }

    //Test process with QFile::copy
    void copyWithQFileCopy()
    {

        process<QtCopyCopier>();
    }

    //Test processThreaded with QFile::copy
    void copyWithQFileThreaded()
    {

        processThreaded<QtCopyCopier>();
    }

#ifdef WIN32
    //Test process with QFile::copy
    void copyWithCopyFileExWin32()
    {

        process<Win32FileExCopier>();
    }

    //Test processThreaded with QFile::copy
    void copyWithCopyFileExWin32Threaded()
    {

        processThreaded<Win32FileExCopier>();
    }
#endif

    //Test process with a memory mapped file
    void copyWithMappedCopy()
    {

        process<MappedCopier>();
    }

    //Test processThreaded with memory mapped copies
    void copyWithMappedCopyThreaded()
    {

        processThreaded<MappedCopier>();
    }

private:

    //Processes a predetermined amount of files with QtConcurrent
    template<typename T,typename = std::enable_if_t<std::is_base_of_v<FileCopier,T>>>
    void processThreaded()
    {
        qInfo() << "Benchmarking " + QString::number(m_Iterations) + " files threaded!!";

        //Get the destination dir and make a full string with the target file
        const QString destinationDir{m_DestDir};
        const QFileInfo fileSource{m_SourceFile};

        //Initialize the copiers and set there source and destination files.
        QList<T> fileCopiers{};

        fileCopiers.reserve(m_Iterations);

        for(size_t i{}; i < m_Iterations; ++i)
        {
            T fileCopier{};
            //TODO rewrite this with QString args funtion for string interpolation.
            //Create an unique name for each copy
            const QString dest{QString(destinationDir)+ "/"+ fileSource.baseName() + "_copy" + QString::number(i) + "." + fileSource.suffix()};

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
                QVERIFY(copier.run());
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
    void process()
    {
        qInfo() << "Benchmarking file!!";

        const QFileInfo fileSource{m_SourceFile};

        //Get the destination dir and make a full string with the target file
        const QString dest{m_DestDir + "/_copy_" +  fileSource.fileName()};

        //Copier initialization
        T fileCopier{};
        fileCopier.setSource(fileSource.absoluteFilePath());
        fileCopier.setDestination(dest);
        //Start the process
        //Not doing this once will result in inacurrate speeds
        //Is there anything to fix this?
        QBENCHMARK_ONCE{
            QVERIFY(fileCopier.run());
        }


        //Check if the destination exists before removal
        const auto& destination{fileCopier.Destination()};
        QVERIFY2(destination.exists(),"Destination folder does not exists anymore!!");


        QFile::remove(destination.absoluteFilePath());
        return;
    }

    const QString m_SourceFile;
    const QString m_DestDir;
    const size_t m_Iterations;
};

#endif // COPIERBENCHMARKS_H
