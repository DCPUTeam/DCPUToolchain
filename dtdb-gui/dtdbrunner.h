#ifndef DTDBRUNNER_H
#define DTDBRUNNER_H

#include <QProcess>
#include <QDebug>

#include <unistd.h>

class DTDBRunner: public QObject
{
    Q_OBJECT

public:
    DTDBRunner();

signals:
    void privateRunCommand(QString);
    void vmUpdated(QList<QString>);

public slots:
    void runCommand(QString);
    void stop();
    void run();

private slots:
    void gotStdout();
    void gotStderr();


private:
    QProcess* process;
    void processVMUpdate(QStringList args);
};

#endif


