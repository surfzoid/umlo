#ifndef PARAMETRES_H
#define PARAMETRES_H

#include <QMainWindow>

namespace Ui {
class Parametres;
}

class Parametres : public QMainWindow
{
    Q_OBJECT

public:
    explicit Parametres(QWidget *parent = nullptr);
    ~Parametres();

private:
    Ui::Parametres *ui;
};

#endif // PARAMETRES_H
