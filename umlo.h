#ifndef UMLO_H
#define UMLO_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Umlo; }
QT_END_NAMESPACE

class Umlo : public QMainWindow
{
    Q_OBJECT

public:
    Umlo(QWidget *parent = nullptr);
    ~Umlo();

private slots:
    void on_actionPr_f_rences_triggered();

private:
    Ui::Umlo *ui;
    QProcess *SftpProc = new QProcess;
    void Init();

};
#endif // UMLO_H
