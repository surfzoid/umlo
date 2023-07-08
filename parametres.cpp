#include "parametres.h"
#include "ui_parametres.h"
#include "umlo.h"
#include <QFileDialog>

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
    Umlo::MloMount = settings.value("MloMount", Umlo::MloMount).value<QString>();

    settings.endGroup();

    ui->UserEd->setText(Umlo::UserName);
    ui->PassEd->setText(Umlo::UserPass);
    ui->PrefixUserEd->setText(Umlo::PrefixUser);
    ui->RpmbuildEd->setText(Umlo::RpmbuildPath);
    ui->RootEd->setText(Umlo::MloMount);
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
    settings.setValue("MloMount", ui->RootEd->text());

    settings.endGroup();
    settings.sync();

    Umlo::UserName = ui->UserEd->text();
    Umlo::UserPass = ui->PassEd->text();
    Umlo::PrefixUser = ui->PrefixUserEd->text();
    Umlo::RpmbuildPath = ui->RpmbuildEd->text();
    Umlo::MloMount = ui->RootEd->text();
    //Umlo().Init();
    close();
}

void Parametres::on_BtnCancel_released()
{
    close();
}

void Parametres::on_BtnRpmbuilDirChoice_released()
{
    QUrl _IntputFolder = QFileDialog::getExistingDirectory(this,
                                                           (tr("Choisir un dossier rpmbuild")), ui->RpmbuildEd->text());

    if (_IntputFolder.isEmpty() == false) {
        ui->RpmbuildEd->setText(_IntputFolder.path().toUtf8());
    }
}

void Parametres::on_BtnSshDirChoice_released()
{
    QUrl _IntputFolder = QFileDialog::getExistingDirectory(this,
                                                           (tr("Choisir un dossier de montage ssh/sftp")), ui->RootEd->text());

    if (_IntputFolder.isEmpty() == false) {
        ui->RootEd->setText(_IntputFolder.path().toUtf8());
    }
}
