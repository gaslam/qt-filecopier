#include "CopierBenchmarks.h"
#include <QtTest>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const QString startProgramAgainMsg{"Start the program again!!"};

    QTextStream qin(stdin);
    QTextStream qout(stdout);

    QString source;
    qout << "Enter the source file you want to copy from!\n(empty input = a default file of 99.5 MB will be used)\n\n " << Qt::flush;
    source = qin.readLine();

    if(source.isEmpty())
    {
        source = "resource.flac";
        qout << source << " at location: " << QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + source)<< " will be used.\n\n" << Qt::flush;
    }

    const QFileInfo sourceFile{source};
    const QDir sourceDir{sourceFile.dir()};

    if(!sourceDir.exists() || !sourceFile.isFile())
    {
        qCritical() << "Source does not exist or is invalid. Enter a valid source file!!\n Check the value for any spelling mistakes.";
        qCritical() << startProgramAgainMsg;
        return -1;
    }

    QString destination;
    qout << "Enter the destination folder you want to copy to!\n(empty value = location of the program itself)\n\n" << Qt::flush;
        destination = qin.readLine();

    if(destination.isEmpty())
    {
        destination = QCoreApplication::applicationDirPath();
        qout << "Chosen folder: " << destination << "\n\n" << Qt::flush;
    }
    QDir destDir{destination};

    if(!destDir.exists())
    {
        qCritical() << "Destination folder does not exist! Enter a valid source folder.\n Check the value for any spelling mistakes.";
        qCritical() << startProgramAgainMsg;
        return -1;
    }

    QString iterationsStr;
    qout << "For test which require multiple copies. How many should be made?\n\n(default = 100)\n\n" << Qt::flush;
    iterationsStr = qin.readLine();

    //Does not go below 0
    size_t iterations{};

    if(iterationsStr.isEmpty())
    {
        iterations = 100;
    }
    else
    {
        iterations = iterationsStr.toInt();
    }

    if(iterations == 0)
    {
        qCritical() << "Invalid amount of iterations!! Minimum accepted value = 1.";
        qCritical() << startProgramAgainMsg;
        return -1;
    }

    CopierBenchmarks benchmarks{iterations,sourceFile.absoluteFilePath(),destDir.absolutePath()};

    return QTest::qExec(&benchmarks, argc, argv);
}
