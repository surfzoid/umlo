#include "umlo.h"
#include "ui_umlo.h"

Umlo::Umlo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Umlo)
{
    ui->setupUi(this);
    Init();
}

Umlo::~Umlo()
{
    delete ui;
    SftpProc->start("umount", QStringList() << "/home/eric/.cache/surfzoid/mlossh");
}

void Umlo::Init()
{
    SftpProc->start("sshfs", QStringList() << "surfzoid@repository.mageialinux-online.org:" << "/home/eric/.cache/surfzoid/mlossh" << "-p" << "2222" << "-C" << "-o" << "auto_unmount");
}

void Umlo::on_actionPr_f_rences_triggered()
{

}
