#include "parametres.h"
#include "ui_parametres.h"

Parametres::Parametres(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parametres)
{
    ui->setupUi(this);
}

Parametres::~Parametres()
{
    delete ui;
}
