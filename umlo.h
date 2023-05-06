#ifndef UMLO_H
#define UMLO_H

#include <QMainWindow>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <qtconcurrentrun.h>
#include <QFutureWatcher>

QT_BEGIN_NAMESPACE
namespace Ui { class Umlo; }
QT_END_NAMESPACE

class Umlo : public QMainWindow
{
    Q_OBJECT

public:
    Umlo(QWidget *parent = nullptr);
    ~Umlo();
    static QString UserName;
    static QString PrefixUser;
    static QString RpmbuildPath;
    static QString RpmbuildPathX1;
    static QString MloMount;
    void Init();

private slots:
    void on_actionPr_f_rences_triggered();

    void on_BtnConect_released();

    void on_BtnDeConect_released();
    void setProgress(QFileInfo FsName);

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void addlabelstatus(QProcess::ProcessState newState);
    void processreadyReadStandardOutput();

private:
    Ui::Umlo *ui;
    QProcess *SftpProc;
    QDir *MountDir;
    QSettings settings;
    void *scanDir( QDir dir );
    QFuture <void> future;
signals:
    void computationProgress( QFileInfo FsName );

};
#endif // UMLO_H
