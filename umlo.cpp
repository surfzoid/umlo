﻿#include "umlo.h"
#include "ui_umlo.h"
#include "parametres.h"
#include "about.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QMessageBox>
#include <QSizeGrip>
#include <QGridLayout>
#include <QtDebug>
#include <QTextBlockGroup>
#include <QTextFormat>
#include <QTextDocument>
#include <QProgressDialog>

QString Umlo::UserName = "Ex:surfzoid";
QString Umlo::UserPass = "hik12345";
QString Umlo::PrefixUser = "surf";
QStringList Umlo::RpmbuildPath; /*= QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/rpmbuild";*/
QString Umlo::MloMount = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";

SimpleCrypt Umlo::crypto;

Umlo::Umlo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Umlo)
{
    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(ui->StatuLbl,1);
    MloMount.append( UserName + "/mlossh");

    SftpProc = new QProcess(this);
    connect(SftpProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){ Umlo::processFinished(exitCode,exitStatus); });

    connect(SftpProc, &QProcess::readyReadStandardOutput, this, &Umlo::processreadyReadStandardOutput);

    //settings//
    QCoreApplication::setOrganizationName("Surfzoid");
    QCoreApplication::setOrganizationDomain("https://github.com/surfzoid");
    QCoreApplication::setApplicationName("umlo");

    QSettings settings;
    crypto.setKey(Q_UINT64_C(0x0c2ad4a4acb9f023 * 3));//some random number
    settings.beginGroup("umlo");
    QStringList keys = settings.allKeys();

    Umlo::UserName = settings.value("UserName", Umlo::UserName).value<QString>();
    Umlo::UserPass = Umlo::crypto.decryptToString(settings.value("Password", "hik12345").value<QString>());
    Umlo::PrefixUser = "." + settings.value("PrefixUser", Umlo::PrefixUser).value<QString>() + ".";
    Umlo::RpmbuildPath = settings.value("RpmbuildPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/rpmbuild").value<QString>().split(";");
    Umlo::MloMount = settings.value("MloMount", Umlo::MloMount).value<QString>();

    settings.endGroup();

    if (UserName == "Ex:surfzoid")
        Umlo::on_actionPr_f_rences_triggered();

    MountDir = new QDir(MloMount);

    futureWatcher = new QFutureWatcher<void>(this);
    //    connect(futureWatcher, &QFutureWatcher<void>::finished, futureWatcher, &QFutureWatcher<void>::deleteLater);
    //    connect( futureWatcher, &QFutureWatcher<void>::finished, this,  Umlo::taskCompleted() ) ;
    connect(futureWatcher, &QFutureWatcher<void>::finished, this, &Umlo::taskCompleted);

    futureWatcher->setFuture(future);

    connect(this, &Umlo::computationProgress, this, &Umlo::setProgress, Qt::QueuedConnection);

    future = QtConcurrent::run(this, &Umlo::FindLocalRpm, RpmbuildPath);

    ui->StatuLbl->setText("Listage des rpms Locaux, veuillez patienter jusqu'a la fin");
    ui->textEdit->append("Liste des RPMS depuis SRPMS, veuillez patienter jusqu'a la fin");

    ui->TableWRpm->setWindowFlags(Qt::SubWindow);

    QSizeGrip * sizeGrip = new QSizeGrip(ui->TableWRpm);

    QGridLayout * layout = new QGridLayout(ui->TableWRpm);
    layout->addWidget(sizeGrip, 0,0,1,1,Qt::AlignBottom | Qt::AlignRight);


    HeightOffset = height() - ui->TableWRpm->height();
    WidthtOffset = width() - ui->TableWRpm->width();
}

Umlo::~Umlo()
{
    SftpProc->startDetached("umount", QStringList() << MountDir->path());
    delete ui;
}


void Umlo::taskCompleted()
{
    //int ret = QMessageBox::warning(this,tr("test "),tr("test"),QMessageBox::Ok | QMessageBox::Cancel);
}

QString CmdLine;
void Umlo::Init()
{
    if (!MountDir->exists()) {
        MountDir->mkpath(MountDir->path());
    }

    MountDir->refresh();
    if (MountDir->count() <= 2) {
        int ret = QMessageBox::warning(this,tr("Confirmation "),tr("Le répertoire de montage ssh ") + MountDir->path()+ tr("/ est vide, essayer de le monter ou annuler pour le monter manuellement."),QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
            return;

        ui->StatuLbl->setText("Connection a " + UserName + "@repository.mageialinux-online.org:2222");

        //        SftpProc->start("sshfs", QStringList() << "--help");

        if (UserPass == "") {
            SftpProc->start("sshfs", QStringList() << UserName + "@repository.mageialinux-online.org:/team-mlo/public_html/www" << MountDir->path() << "-p" << "2222" << "-C");
            CmdLine = SftpProc->program();
        }else{
            SftpProc->start("SSHPASS=\"" + UserPass + "\"", QStringList() << "sshfs" << "repository.mageialinux-online.org:/team-mlo/public_html/www" << MountDir->path() << "-p" << "2222" << "-C" << "-o" << "ssh_command=\"sshpass" << "-e" << "ssh" << "-l" << UserName + "\"");
        }
    }else{
        ui->StatuLbl->setText("Connection ok");

        ui->textEdit->append("Recuperation de la liste des RPMS depuis le serveur, veuillez patienter jusqu'a la fin...");

        RpmName = "";
        PrCase=0;
        future = QtConcurrent::run(this, &Umlo::scanDir, MountDir->path());
    }
}

void Umlo::on_actionPr_f_rences_triggered()
{
    Parametres *setTing = new Parametres();
    setTing->setWindowModality(Qt::WindowModality::WindowModal);
    setTing->show();

}

void Umlo::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && SftpProc->program() == "sshfs") {
        MountDir->refresh();
        if (MountDir->count() <= 2) {
            ui->StatuLbl->setText("Echec de connection");
        }else{
            ui->StatuLbl->setText("Connection ok");

            ui->textEdit->append("Recuperation de la liste des RPMS depuis le serveur, veuillez patienter jusqu'a la fin...");

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
        emit computationProgress(fi);

    QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase );
    foreach ( QFileInfo di, dil )
        scanDir( QDir( di.absoluteFilePath() ) );

}

void Umlo::setProgress(QFileInfo FsName)
{
    QStringList RName = FsName.fileName().split("-");

    switch(PrCase) {
    case 0:
        Populate(FsName.fileName(), "SFTP", "Aucun","");
        if (future.isFinished())
        {
            ui->textEdit->append("Fin de la liste");
        }
        break;

    case 1:
        QFile::remove(FsName.path() + "/" + FsName.fileName() );
        Populate(FsName.fileName(), "SFTP", "suprimé","");
        break;
    default:
        PrCase=-1;
        break;
    };
}

void Umlo::on_BtnDel_released()
{
    MountDir->refresh();
    if (MountDir->count() <= 2) {
        QMessageBox::critical(this,tr("Confirmation "),tr("Le répertoire de montage ssh ") + MountDir->path()+ tr("/ est vide, essayer de le monter."),QMessageBox::Ok);
        return;
    }

    int ret = QMessageBox::warning(this,tr("Confirmation "),tr("Cette action vas effacer sur le serveur tous les rpms et srpm de ") + ui->CmbxRpmList->currentText(),QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Cancel)
        return;

    PrCase=1;

    RpmName = ui->CmbxRpmList->currentText();
    future = QtConcurrent::run(this, &Umlo::scanDir, MountDir->path());
    ui->textEdit->append("Suppression des RPMS " + ui->CmbxRpmList->currentText() + " sur le serveur, veuillez patienter jusqu'a la fin");

}

void Umlo::on_CmbxRpmList_textActivated(const QString &arg1)
{
    ui->CmbxRpmVers->clear();

    foreach (QString Path, RpmbuildPath) {
        QDir dir(Path);
        dir.refresh();
    }

    if (LocalRpmEnd) {

        future = QtConcurrent::run(this, &Umlo::FindLocalRpmVers, RpmbuildPath, arg1);
    }

    //    FindLocalRpmVers(MloMount, arg1);
}

void Umlo::FindLocalRpmVers(QStringList dirlist, QString LocalRpm)
{
    foreach (QString Path, dirlist) {
        QDir dir( Path);
        QFileInfoList fil = dir.entryInfoList( QStringList( LocalRpm + "*" + PrefixUser + "*" ),QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
        foreach ( QFileInfo fi, fil )
        {
            QStringList RpmVers = fi.fileName().split(PrefixUser).at(0).split("-");
            int Last = RpmVers.length();
            QString Rel = RpmVers.at(Last - 1);
            QString VersRel = RpmVers.at(Last - 2) + "-" + Rel;
            if (ui->CmbxRpmVers->findText(VersRel) == -1)
                ui->CmbxRpmVers->addItem(VersRel);
        }

        QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
        foreach ( QFileInfo di, dil )
            FindLocalRpmVers( QStringList( di.absoluteFilePath() ) , LocalRpm);

        ui->CmbxRpmVers->model()->sort(0, Qt::DescendingOrder);
        ui->CmbxRpmVers->setCurrentIndex(0);
    }
}


void Umlo::FindLocalRpm(QStringList dirlist)
{
    foreach (QString Path, dirlist) {
        QDir dir( Path);
        QFileInfoList fil = dir.entryInfoList( QStringList( "*" + PrefixUser + "*rpm" ),QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
        for (int i = 0; i < fil.length(); i++ )
            //    foreach ( QFileInfo fi, fil )
        {
            QFileInfo fi( fil.at(i) );
            //        QStringList RName = fi.fileName().split("-");
            QStringList RpmVers = fi.fileName().split(PrefixUser).at(0).split("-");
            int Last = RpmVers.length();
            QString Rel = RpmVers.at(Last - 1);
            QString VersRel = RpmVers.at(Last - 2) + "-" + Rel;
            QString RName = fi.fileName().split("-").at(0);
            //        QString RName = FindRName.left(FindRName.length() - 1);
            switch(UpCase) {
            case 0:
                LocalRpmEnd = false;
                if (ui->textEdit->textColor() == Qt::black) {
                    ui->textEdit->setTextColor(Qt::blue);
                }else{
                    ui->textEdit->setTextColor(Qt::black);
                }

                Populate(fi.fileName(), "Local", "Aucun",Path);
                if (ui->CmbxRpmList->findText(RName) == -1)
                {
                    ui->CmbxRpmList->addItem(RName);
                    if (ui->CmbxRpmVers->count() == 0)
                        on_CmbxRpmList_textActivated(RName);
                    ui->CmbxRpmList->model()->sort(0, Qt::AscendingOrder); // default Qt::AscendingOrder
                    ui->CmbxRpmList->setCurrentIndex(0);
                }

                if (i == fil.length() - 1 and ui->CmbxRpmList->count() > 0) {
                    LocalRpmEnd = true;
                    ui->CmbxRpmVers->clear();
                    FindLocalRpmVers(dirlist, ui->CmbxRpmList->itemText(0));

                    ui->StatuLbl->setText("Fin du listage des rpms Locaux");
                }
                break;

            case 1:
                if (RName == ui->CmbxRpmList->currentText() and VersRel == ui->CmbxRpmVers->currentText() )
                    UploadRpm(fi);
                break;
            default:
                UpCase=-1;
                break;
            };

        }

        QFileInfoList dil = dir.entryInfoList( QStringList( "*" ),QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks,QDir::Name | QDir::IgnoreCase );
        foreach ( QFileInfo di, dil )
            FindLocalRpm( QStringList( di.absoluteFilePath() ) );
    }
}

void Umlo::on_actionRafraichir_triggered()
{
    UpCase=0;
    ui->CmbxRpmVers->clear();
    ui->CmbxRpmList->clear();
    //    ui->textEdit->clear();
    clearitems();

    future = QtConcurrent::run(this, &Umlo::FindLocalRpm, RpmbuildPath);
    //    FindLocalRpm(MloMount);
}

void Umlo::on_BtnSend_released()
{
    MountDir->refresh();
    if (MountDir->count() <= 2) {
        QMessageBox::critical(this,tr("Confirmation "),tr("Le répertoire de montage ssh ") + MountDir->path()+ tr("/ est vide, essayer de le monter."),QMessageBox::Ok);
        return;
    }
    UpCase=1;
    FindLocalRpm(RpmbuildPath);
    //FindLocalRpm(MloMount);
    //future = QtConcurrent::run(this, &Umlo::FindLocalRpm, RpmbuildPath);

}

void Umlo::UploadRpm(QFileInfo Fs)
{
    ui->textEdit->append("Envoi de " + Fs.fileName());
    QString MloVers = Fs.fileName().split(PrefixUser).at(1).split(".").at(0);
    QString MloVersDir = MloVers.right(1);
    QString Arch = Fs.fileName().split(MloVers).at(1).split(".").at(1);
    QString DestDir;
    if (Arch == "src") {
        DestDir = MountDir->absolutePath() + "/" + MloVersDir + "/SRPMS/media/core/";
    }else{
        DestDir = MountDir->absolutePath() + "/" + MloVersDir + "/" + Arch + "/media/core/";
    }

    bool FailCp = false;
    QFile fromFile(Fs.absoluteFilePath());
    QFile toFile(DestDir + Fs.fileName());


    bool toFileexists = toFile.exists();
    if (toFileexists) {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(tr("Le fichier ") + Fs.fileName() + tr(" existe deja sur le serveur."));
        ui->textEdit->setTextColor(Qt::black);
        future.cancel();
        return;
    }

    qint64 nCopySize = fromFile.size();
    qint64 BuffCpy = 1048576;
    qint64 SizeMB = nCopySize/1024/1024;
    if (nCopySize<BuffCpy)
        BuffCpy=1024;

    QProgressDialog progress("Copie " + Fs.fileName() + " " + QString::number(SizeMB ,10) + " MB", "Annuler la copie", 0,nCopySize, this);
    progress.setWindowModality(Qt::ApplicationModal);

    fromFile.open(QIODevice::ReadOnly);
    toFile.open(QIODevice::WriteOnly);
    for (qint64 i = 1; i < nCopySize; i = i+BuffCpy) {
        if (!toFile.write(fromFile.read(BuffCpy))) {
            ui->textEdit->append(tr("Erreur pendand l'écriture"));
            break;
        }

        fromFile.seek(i+BuffCpy - 1);  // move to next  read
        toFile.seek(i+BuffCpy - 1); // move to next  write
        //        ui->textEdit->append(QString::number(fromFile.pos()));
        progress.setValue(i);

        //        toFile.flush();
        //fromFile.commitTransaction();
        if (progress.wasCanceled())
        {

            Populate(Fs.fileName(), "SFTP", "Annuler","");
            FailCp = true;
            break;
        }
    }
    toFile.setPermissions(fromFile.permissions());
    progress.setValue(nCopySize);
    if (!FailCp)
        Populate(Fs.fileName(), "SFTP", "copié","");

    fromFile.close();
    toFile.close();

    //    if (FileCopy->copy(Fs.absoluteFilePath(), DestDir + Fs.fileName())) {
    //        Populate(Fs.fileName(), "SFTP", "copié");
    //    }else{
    //        Populate(Fs.fileName(), "SFTP", "Echec de copie");
    //    }
    ui->textEdit->setTextColor(Qt::black);
}

void Umlo::on_BtnClearTxt_released()
{
    //    ui->textEdit->clear();
    clearitems();
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

void Umlo::on_actionA_propos_triggered()
{
    About *Abt = new About();
    Abt->show();
}

void Umlo::on_TextFilter_textChanged(const QString &arg1)
{
    for (int i=0; i< ui->TableWRpm->rowCount(); i++) {
        ui->TableWRpm->showRow(i);
        if (arg1.length() > 0) {
            QTableWidgetItem *RpmItem = ui->TableWRpm->takeItem(i, 0);
            RpmItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
            if (!RpmItem->text().contains(arg1))
            {
                ui->TableWRpm->hideRow(i);
            }
            ui->TableWRpm->setItem(i, 0, RpmItem);
        }
    }

    ui->TableWRpm->selectRow(ui->TableWRpm->rowCount() - 1);
}

void Umlo::clearitems()
{
    //clear the table items of file list
    for (int i=0; i< ui->TableWRpm->rowCount(); i++)
    {
        for (int j=0; j< ui->TableWRpm->columnCount(); j++ )
        {
            delete ui->TableWRpm->takeItem(i, j);
        }
        ui->TableWRpm->removeRow(i);
    }
    ui->TableWRpm->setRowCount(0);
    return;
}

void Umlo::Populate(QString fileName, QString Whereis, QString Statu, QString Path)
{
    ui->TableWRpm->horizontalHeader()->sortIndicatorOrder();

    QString RpmName = fileName.split(PrefixUser).at(0);
    //RpmName = RpmName.remove(RpmName.length() - 1,1);
    QString MloVers = fileName.split(PrefixUser).at(1).split(".").at(0);
    QStringList ArchType = fileName.split(MloVers).at(1).split(".");
    QString Arch = ArchType.at(1);
    QString TypeRpm = "rpm";
    if (fileName.contains("src"))TypeRpm = "SRPM";

    QTableWidgetItem *RpmItem = new QTableWidgetItem(RpmName);
    RpmItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
    if (Path != "")
        RpmItem->setToolTip(Path);

    QTableWidgetItem *VersItem = new QTableWidgetItem(MloVers);
    VersItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);

    QTableWidgetItem *ArchItem = new QTableWidgetItem(Arch);
    ArchItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);

    QTableWidgetItem *TypeItem = new QTableWidgetItem(TypeRpm);
    TypeItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);

    QTableWidgetItem *WhereIsItem = new QTableWidgetItem(Whereis);
    WhereIsItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);

    QTableWidgetItem *StatusItem = new QTableWidgetItem(Statu);
    StatusItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);

    if (Whereis == "SFTP") {
        RpmItem->setBackground(Qt::darkRed);
        RpmItem->setForeground(Qt::white);
    }
    TypeItem->setBackground(Qt::cyan);
    ArchItem->setBackground(Qt::red);
    ArchItem->setForeground(Qt::white);

    if (Statu.contains("Echec") or Statu.contains("sup"))
    {
        StatusItem->setForeground(Qt::red);
    }else{
        StatusItem->setForeground(Qt::darkGreen);
    }

    int row = ui->TableWRpm->rowCount();
    ui->TableWRpm->insertRow(row);
    ui->TableWRpm->setItem(row, 0, RpmItem);
    ui->TableWRpm->setItem(row, 1, VersItem);
    ui->TableWRpm->setItem(row, 2, ArchItem);
    ui->TableWRpm->setItem(row, 3, TypeItem);
    ui->TableWRpm->setItem(row, 4, WhereIsItem);
    ui->TableWRpm->setItem(row, 5, StatusItem);

    QHeaderView *header = ui->TableWRpm->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    int CurRow = ui->TableWRpm->rowCount() + 1;
    QTableWidgetItem *item=new QTableWidgetItem(windowIcon(),QString::number(row),0);//set Icon a and string 1

    ui->TableWRpm->setVerticalHeaderItem(row,item);

    ui->TableWRpm->scrollToBottom();
    return;

}

void Umlo::on_actionQT_infos_triggered()
{
    QApplication::aboutQt();
}

void Umlo::resizeEvent(QResizeEvent *event)
{
    ui->TableWRpm->resize(width() - WidthtOffset, height() - HeightOffset);
}
