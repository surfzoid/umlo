#include "parametres.h"
#include "ui_parametres.h"
#include "umlo.h"
#include <QFileDialog>
#include <QStandardPaths>

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
    Umlo::RpmbuildPath = settings.value("RpmbuildPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/rpmbuild").value<QString>().split(";");
    Umlo::MloMount = settings.value("MloMount", Umlo::MloMount).value<QString>();

    settings.endGroup();

    ui->UserEd->setText(Umlo::UserName);
    ui->PassEd->setText(Umlo::UserPass);
    ui->PrefixUserEd->setText(Umlo::PrefixUser);
//    ui->RpmbuildEd->setText(Umlo::RpmbuildPath);
    QStringList RpmBuildPathList = Umlo::RpmbuildPath;
    foreach (QString Path, RpmBuildPathList)
        ui->comboBoxRpmbuildDir->addItem(Path);
    ui->RootEd->setText(Umlo::MloMount);
}

void Parametres::showEvent(QShowEvent *event)
{
//    ui->BtnSshDirChoice->y() = ui->RootEd->y();
//    ui->gridLayout->setDefaultPositioning(ui->formLayoutWidget->y(),Qt::Orientation::Horizontal);
//    ui->gridLayout->setAlignment(ui->formLayout->alignment());
}

Parametres::~Parametres()
{
    delete ui;
}

void Parametres::on_BtnSave_released()
{

    Umlo::UserName = ui->UserEd->text();
    Umlo::UserPass = ui->PassEd->text();
    Umlo::PrefixUser = ui->PrefixUserEd->text();
    QString RpmbuildPathClean = CmBxToStr();
    QStringList test = Umlo::RpmbuildPath = RpmbuildPathClean.split(";");
    Umlo::MloMount = ui->RootEd->text();
    settings.beginGroup("umlo");

    settings.setValue( "UserName", Umlo::UserName);
    settings.setValue("Password",Umlo::crypto.encryptToString(Umlo::UserPass) );
    settings.setValue( "PrefixUser", Umlo::PrefixUser);
    settings.setValue("RpmbuildPath", RpmbuildPathClean);
    settings.setValue("MloMount", Umlo::MloMount);

    settings.endGroup();
    settings.sync();
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
                                                           (tr("Choisir un dossier rpmbuild")), ui->comboBoxRpmbuildDir->currentText());

    if (_IntputFolder.isEmpty() == false) {
//        ui->RpmbuildEd->setText(_IntputFolder.path().toUtf8());
        ui->comboBoxRpmbuildDir->addItem(_IntputFolder.path().toUtf8());
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

void Parametres::on_BtnRpmbuilDirDel_released()
{
    ui->comboBoxRpmbuildDir->removeItem(ui->comboBoxRpmbuildDir->currentIndex());
}

QString Parametres::CmBxToStr()
{
    QString ListDir;
    for (int i = 0; i < ui->comboBoxRpmbuildDir->count(); i++ )
        ListDir += ui->comboBoxRpmbuildDir->itemText(i) + ";";

    return ListDir.remove(ListDir.length() - 1,1);
}
