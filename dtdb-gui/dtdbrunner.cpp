#include "dtdbrunner.h"

DTDBRunner::DTDBRunner() : QObject()
{
    QStringList arguments;

    process = new QProcess(this);
    process->start("./dtdb/dtdb", arguments);

    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(gotStdout()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(gotStderr()));
    connect(this, SIGNAL(privateRunCommand(QString)), this, SLOT(runCommand(QString)));
}


void DTDBRunner::runCommand(QString s)
{
    process->write(s.toAscii() + "\n");
}

void DTDBRunner::gotStderr()
{
    QByteArray data = process->readAllStandardError();
    QString str(data);
    
    QStringList lines = str.split("\n", QString::SkipEmptyParts);

    foreach(QString line, lines)
    {
	if(line.indexOf(':') != -1)
	{
	    QStringList args = line.split(":", QString::SkipEmptyParts);
	    if(args[0] == "vm")
	    {
		processVMUpdate(args);
	    }
	}
    }
}

void DTDBRunner::gotStdout()
{
    QByteArray data = process->readAllStandardOutput();
    QString str(data);

}

void DTDBRunner::stop()
{
    process->terminate();
}

void DTDBRunner::run()
{
    emit privateRunCommand("run");
}

void DTDBRunner::processVMUpdate(QStringList args)
{
    QList<QString> registers;
    foreach(QString reg, args)
    {
	int number = reg.toInt();
	QString r = QString("0x%1").arg(number, 4, 16, QChar('0'));
	registers.append(r);
    }
    emit vmUpdated(registers);
}

