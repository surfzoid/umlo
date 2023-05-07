#include "umlo.h"
#include "ui_umlo.h"
#include "parametres.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QMessageBox>
#include <QSizeGrip>
#include <QGridLayout>

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

    auto futureWatcher = new QFutureWatcher<void>(this);
    connect(futureWatcher, &QFutureWatcher<void>::finished, futureWatcher, &QFutureWatcher<void>::deleteLater);

    futureWatcher->setFuture(future);
    connect(this, &Umlo::computationProgress, this, &Umlo::setProgress, Qt::QueuedConnection);

    FindLocalRpm(RpmbuildPath);
    FindLocalRpm(RpmbuildPathX1);

    ui->textEdit->setWindowFlags(Qt::SubWindow);


    QSizeGrip * sizeGrip = new QSizeGrip(ui->textEdit);

    QGridLayout * layout = new QGridLayout(ui->textEdit);
    layout->addWidget(sizeGrip, 0,0,1,1,Qt::AlignBottom | Qt::AlignRight);


}

Umlo::~Umlo()
{
    SftpProc->startDetached("umount", QStringList() << MountDir->path());
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

            ui->textEdit->append("Recuperation de la liste des RPMS depuis le serveur...");

            RpmName = "";
            PrCase=0;
            future = QtConcurrent::run(this, &Umlo::scanDir, MountDir->path());

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
    PrCase=0;
    Init();
}

void Umlo::on_BtnDeConect_released()
{
    SftpProc->start("umount", QStringList() << MountDir->path());

}

void Umlo::scanDir(QDir dir)
{
    QFileInfoList fil = dir.entryInfoList( QStringList( RpmName + "*" + PrefixUser + "*rpm" ), QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo fi, fil )
        //ui->textEdit->append(fi.fileName());
        emit computationProgress(fi);

    QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo di, dil )
        scanDir( QDir( di.absoluteFilePath() ) );

}

void Umlo::setProgress(QFileInfo FsName)
{
    if (ui->textEdit->textColor() == Qt::black) {
        ui->textEdit->setTextColor(QColor::fromRgb(255, 165, 0));
    }else{
        ui->textEdit->setTextColor(Qt::black);
    }
    ui->textEdit->append(FsName.fileName());
    QStringList RName = FsName.fileName().split("-");

    switch(PrCase) {
    case 0:

        if (future.isFinished())
        {
            ui->textEdit->append("Fin de la liste");
        }
        break;

    case 1:
        QFile::remove(FsName.path() + "/" + FsName.fileName() );
        ui->textEdit->append("suprimé");
        //ui->CmbxRpmList->removeItem(ui->CmbxRpmList->findText(RName.at(0)));
        break;
    default:
        PrCase=-1;
        break;
    };
}

void Umlo::on_BtnDel_released()
{
    PrCase=1;

    int ret = QMessageBox::warning(this,tr("Confirmation "),tr("Cette action vas effacer sur le serveur tous les rpms et srpm de ") + ui->CmbxRpmList->currentText(),QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    RpmName = ui->CmbxRpmList->currentText();
    future = QtConcurrent::run(this, &Umlo::scanDir, MountDir->path());
    ui->textEdit->append("Suppression des RPMS " + ui->CmbxRpmList->currentText() + " sur le serveur");

}

void Umlo::on_CmbxRpmList_textActivated(const QString &arg1)
{
    ui->CmbxRpmVers->clear();
    FindLocalRpmVers(RpmbuildPath, arg1);
    FindLocalRpmVers(RpmbuildPathX1, arg1);
}

void Umlo::FindLocalRpmVers(QDir dir, QString LocalRpm)
{
    QFileInfoList fil = dir.entryInfoList( QStringList( LocalRpm + "*" + PrefixUser + "*" ),QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo fi, fil )
    {
        QStringList RpmVers = fi.fileName().split(PrefixUser).at(0).split("-");
        int Last = RpmVers.length();
        QString Rel = RpmVers.at(Last - 1);
        QString VersRel = RpmVers.at(Last - 2) + "-" + Rel.remove(1,1);
        if (ui->CmbxRpmVers->findText(VersRel) == -1)
            ui->CmbxRpmVers->addItem(VersRel);
    }

    QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo di, dil )
        FindLocalRpmVers( QDir( di.absoluteFilePath() ) , LocalRpm);
}


void Umlo::FindLocalRpm(QDir dir)
{
    QFileInfoList fil = dir.entryInfoList( QStringList( "*" + PrefixUser + "*rpm" ),QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo fi, fil )
    {
        QStringList RName = fi.fileName().split("-");
        QStringList RpmVers = fi.fileName().split(PrefixUser).at(0).split("-");
        int Last = RpmVers.length();
        QString Rel = RpmVers.at(Last - 1);
        QString VersRel = RpmVers.at(Last - 2) + "-" + Rel.remove(1,1);
        switch(UpCase) {
        case 0:
            if (ui->textEdit->textColor() == Qt::black) {
                ui->textEdit->setTextColor(Qt::blue);
            }else{
                ui->textEdit->setTextColor(Qt::black);
            }

            ui->textEdit->append(fi.fileName());

            if (ui->CmbxRpmList->findText(RName.at(0)) == -1)
            {
                ui->CmbxRpmList->addItem(RName.at(0));
                if (ui->CmbxRpmVers->count() == 0)
                    on_CmbxRpmList_textActivated(RName.at(0));
            }
            break;

        case 1:
            if (RName.at(0) == ui->CmbxRpmList->currentText() and VersRel == ui->CmbxRpmVers->currentText() )
                UploadRpm(fi);
            break;
        default:
            UpCase=-1;
            break;
        };

    }

    QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo di, dil )
        FindLocalRpm( QDir( di.absoluteFilePath() ) );
}
void Umlo::on_actionRafraichir_triggered()
{
    UpCase=0;
    ui->CmbxRpmList->clear();
    ui->textEdit->clear();
    FindLocalRpm(RpmbuildPath);
    FindLocalRpm(RpmbuildPathX1);
}

void Umlo::on_BtnSend_released()
{
    UpCase=1;
    FindLocalRpm(RpmbuildPath);
    //FindLocalRpm(RpmbuildPathX1);

}

void Umlo::UploadRpm(QFileInfo Fs)
{
    ui->textEdit->append("Envoi de " + Fs.fileName());
    QString MloVers = Fs.fileName().split(PrefixUser).at(1).split(".").at(1);
    QString MloVersDir = MloVers.right(1);
    QString Arch = Fs.fileName().split(MloVers).at(1).split(".").at(1);
    QString DestDir;
    if (Arch == "src") {
        DestDir = MloMount + UserName + "/mlossh/" + MloVersDir + "/SRPMS/media/core/";
    }else{
        DestDir = MloMount + UserName + "/mlossh/" + MloVersDir + "/" + Arch + "/media/core/";
    }

    if (QFile::copy(Fs.absoluteFilePath(), DestDir + Fs.fileName())) {
        ui->textEdit->setTextColor(Qt::green);
        ui->textEdit->append(Fs.fileName() + " copié");
    }else{
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append("Echec de copie " + Fs.fileName());
    }
    ui->textEdit->setTextColor(Qt::black);
}

void Umlo::on_BtnClearTxt_released()
{
    ui->textEdit->clear();
}

void Umlo::on_BtnZoomOut_released()
{
    ui->textEdit->zoomOut(1);
}

void Umlo::on_BtnZoomIn_released()
{
    ui->textEdit->zoomIn(1);
}

//void Umlo::closeEvent(QCloseEvent *event)
//{
//}


void Umlo::on_actionRetour_a_la_line_triggered(bool checked)
{
    if (checked) {
       ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    }else{
        ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
    }

}
