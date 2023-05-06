#include "umlo.h"
#include "ui_umlo.h"
#include "parametres.h"
#include <QStandardPaths>
#include <QDirIterator>

QString Umlo::UserName = "Ex:surfzoid";
QString Umlo::PrefixUser = "surf";
QString Umlo::RpmbuildPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/rpmbuild";
QString Umlo::RpmbuildPathX1 = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/rpmbuild/mga9";
QString Umlo::MloMount = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";

Umlo::Umlo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Umlo)
{
    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(ui->StatuLbl,1);

    SftpProc = new QProcess(this);
    connect(SftpProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){ Umlo::processFinished(exitCode,exitStatus); });

    connect(SftpProc, &QProcess::stateChanged, this, &Umlo::addlabelstatus);
    connect(SftpProc, &QProcess::readyReadStandardOutput, this, &Umlo::processreadyReadStandardOutput);

    settings.beginGroup("umlo");
    QStringList keys = settings.allKeys();

    Umlo::UserName = settings.value("UserName", Umlo::UserName).value<QString>();
    Umlo::PrefixUser = settings.value("PrefixUser", Umlo::PrefixUser).value<QString>();
    Umlo::RpmbuildPath = settings.value("RpmbuildPath", Umlo::RpmbuildPath).value<QString>();
    Umlo::RpmbuildPathX1 = settings.value("RpmbuildPathX1", Umlo::RpmbuildPathX1).value<QString>();

    settings.endGroup();

    if (UserName == "Ex:surfzoid")
        Umlo::on_actionPr_f_rences_triggered();

    MountDir = new QDir(MloMount + UserName + "/mlossh");
}

Umlo::~Umlo()
{
    SftpProc->start("umount", QStringList() << MountDir->path());
    delete ui;
}

void Umlo::Init()
{
    if (!MountDir->exists()) {
        MountDir->mkpath(MountDir->path());
    }
    ui->StatuLbl->setText("Connection a " + UserName + "@repository.mageialinux-online.org:2222");

    SftpProc->start("sshfs", QStringList() << UserName + "@repository.mageialinux-online.org:" << MountDir->path() << "-p" << "2222" << "-C");
}

void Umlo::on_actionPr_f_rences_triggered()
{
    Parametres *setTing = new Parametres();
    setTing->setWindowModality(Qt::WindowModality::WindowModal);
    setTing->show();

}

void Umlo::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 1 && SftpProc->program() == "sshfs") {
        if (MountDir->count() == 0) {
            ui->StatuLbl->setText("Echec de connection");
        }else{
            ui->StatuLbl->setText("Connection ok");
            /*QStringList filters;
                 filters << "*surf*";
                 MountDir->setNameFilters(filters);
            QFileInfoList Test = MountDir->entryInfoList();
            QDirIterator it(MountDir->path(), QStringList() << "*surf*", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QFile f(it.next());
                f.open(QIODevice::ReadOnly);

                ui->textEdit->append(f.fileName());
            }*/

            ui->textEdit->append("Recuperation de la liste des RPMS ...");

            auto futureWatcher = new QFutureWatcher<void>(this);
            connect(futureWatcher, &QFutureWatcher<void>::finished, futureWatcher, &QFutureWatcher<void>::deleteLater);
            future = QtConcurrent::run(this, &Umlo::scanDir, MountDir->path());

            futureWatcher->setFuture(future);
            connect(this, &Umlo::computationProgress, this, &Umlo::setProgress, Qt::QueuedConnection);

        }
    }
}

void Umlo::processreadyReadStandardOutput()
{
    //ui->TxtDebug->append(SftpProc->readAllStandardOutput());

}

void Umlo::addlabelstatus(QProcess::ProcessState newState)
{

}


void Umlo::on_BtnConect_released()
{
    Init();
}

void Umlo::on_BtnDeConect_released()
{
    ui->CmbxRpmList->clear();
    ui->textEdit->clear();
    SftpProc->start("umount", QStringList() << MountDir->path());

}

void *Umlo::scanDir( QDir dir )
{ QFileInfoList fil = dir.entryInfoList( QStringList( "*" + PrefixUser + "*" ),
                                         QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                                         QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo fi, fil )
        //ui->textEdit->append(fi.fileName());
        emit computationProgress(fi);

    QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),
                                           QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                                           QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo di, dil )
        scanDir( QDir( di.absoluteFilePath() ) );

}

void Umlo::setProgress(QFileInfo FsName)
{
    ui->textEdit->append(FsName.fileName());
    QStringList RpmName = FsName.fileName().split("-");
    if (ui->CmbxRpmList->findText(RpmName.at(0)) == -1)
        ui->CmbxRpmList->addItem(RpmName.at(0),FsName.path() + FsName.fileName());

    if (future.isFinished())
        ui->textEdit->append("Fin de la list");
}
