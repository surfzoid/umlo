#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    QString Vers = APP_VERSION;
    ui->LblVers->setText("Version " + Vers);
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_released()
{
    QDesktopServices::openUrl(QUrl("https://www.mageialinux-online.org/wiki/depots-supplementaires-pour-mageia-mlo-repository", QUrl::TolerantMode));
}
