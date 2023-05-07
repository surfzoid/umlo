#include "about.h"
#include "ui_about.h"

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
