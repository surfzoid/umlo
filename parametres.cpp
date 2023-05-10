#include "parametres.h"
#include "ui_parametres.h"
#include "umlo.h"

Parametres::Parametres(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parametres)
{
    ui->setupUi(this);

    QSettings settings;
    QString FsSettings =  settings.fileName();
    settings.beginGroup("umlo");

    Umlo::UserName = settings.value("UserName", Umlo::UserName).value<QString>();
    Umlo::UserPass = Umlo::crypto.decryptToString(settings.value("Password", "hik12345").value<QString>());
    Umlo::PrefixUser = settings.value("PrefixUser", Umlo::PrefixUser).value<QString>();
    Umlo::RpmbuildPath = settings.value("RpmbuildPath", Umlo::RpmbuildPath).value<QString>();
    Umlo::RpmbuildPathX1 = settings.value("RpmbuildPathX1", Umlo::RpmbuildPathX1).value<QString>();

    settings.endGroup();

    ui->UserEd->setText(Umlo::UserName);
    ui->PassEd->setText(Umlo::UserPass);
    ui->PrefixUserEd->setText(Umlo::PrefixUser);
    ui->RpmbuildEd->setText(Umlo::RpmbuildPath);
    ui->Rpmbuildx1Ed->setText(Umlo::RpmbuildPathX1);
}

Parametres::~Parametres()
{
    delete ui;
}

void Parametres::on_BtnSave_released()
{
    settings.beginGroup("umlo");

    settings.setValue( "UserName", ui->UserEd->text());
    settings.setValue("Password",Umlo::crypto.encryptToString(ui->PassEd->text()) );
    settings.setValue( "PrefixUser", ui->PrefixUserEd->text());
    settings.setValue("RpmbuildPath", ui->RpmbuildEd->text());
    settings.setValue("RpmbuildPathX1", ui->Rpmbuildx1Ed->text());

    settings.endGroup();
    settings.sync();

    Umlo::UserName = ui->UserEd->text();
    Umlo::UserPass = ui->PassEd->text();
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
