#include "parametres.h"
#include "ui_parametres.h"
#include "umlo.h"

Parametres::Parametres(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parametres)
{
    ui->setupUi(this);

    settings.beginGroup("umlo");
    QStringList keys = settings.allKeys();

    Umlo::UserName = settings.value("UserName", Umlo::UserName).value<QString>();
    Umlo::PrefixUser = settings.value("PrefixUser", Umlo::PrefixUser).value<QString>();
    Umlo::RpmbuildPath = settings.value("RpmbuildPath", Umlo::RpmbuildPath).value<QString>();
    Umlo::RpmbuildPathX1 = settings.value("RpmbuildPathX1", Umlo::RpmbuildPathX1).value<QString>();

    ui->UserEd->setText(Umlo::UserName);
    ui->PrefixUserEd->setText(Umlo::PrefixUser);
    ui->RpmbuildEd->setText(Umlo::RpmbuildPath);
    ui->Rpmbuildx1Ed->setText(Umlo::RpmbuildPathX1);

    settings.endGroup();
}

Parametres::~Parametres()
{
    delete ui;
}

void Parametres::on_BtnSave_released()
{
    settings.beginGroup("umlo");

    settings.setValue( "UserName", ui->UserEd->text());
    settings.setValue( "PrefixUser", ui->PrefixUserEd->text());
    settings.setValue("RpmbuildPath", ui->RpmbuildEd->text());
    settings.setValue("RpmbuildPathX1", ui->Rpmbuildx1Ed->text());

    settings.endGroup();
    settings.sync();

    Umlo::UserName = ui->UserEd->text();
    Umlo::PrefixUser = ui->PrefixUserEd->text();
    Umlo::RpmbuildPath = ui->RpmbuildEd->text();
    Umlo::RpmbuildPathX1 = ui->Rpmbuildx1Ed->text();
    //Umlo().Init();
    close();
}

void Parametres::on_BtnCancel_released()
{
    close();
}
