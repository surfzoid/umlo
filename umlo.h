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
    void setProgress(QFileInfo FsName );

    void on_BtnDel_released();

    void on_CmbxRpmList_textActivated(const QString &arg1);

    void on_actionRafraichir_triggered();

    void on_BtnSend_released();

    void on_BtnClearTxt_released();

    void on_BtnZoomOut_released();

    void on_BtnZoomIn_released();

    void on_actionRetour_a_la_line_triggered(bool checked);

    void on_actionA_propos_triggered();

public slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void addlabelstatus(QProcess::ProcessState newState);
    void processreadyReadStandardOutput();

private:
    Ui::Umlo *ui;
    QProcess *SftpProc;
    QDir *MountDir;
    QSettings settings;
    void scanDir(QDir dir);
    QFuture <void> future;
    QString RpmName;
    int PrCase = 0;
    int UpCase = 0;
    QFileInfoList RpmList;
    void FindLocalRpmVers(QDir dir, QString LocalRpm);
    void FindLocalRpm(QDir dir);
    void UploadRpm(QFileInfo Fs);

signals:
    void computationProgress( QFileInfo FsName);

protected:
    //void closeEvent(QCloseEvent *event) override;

};
#endif // UMLO_H
