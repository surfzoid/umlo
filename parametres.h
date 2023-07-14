#ifndef PARAMETRES_H
#define PARAMETRES_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
class Parametres;
}

class Parametres : public QMainWindow
{
    Q_OBJECT

public:
    explicit Parametres(QWidget *parent = nullptr);
    ~Parametres();

private slots:
    void on_BtnSave_released();

    void on_BtnCancel_released();

    void on_BtnRpmbuilDirChoice_released();

    void on_BtnSshDirChoice_released();

    void on_BtnRpmbuilDirDel_released();

private:
    Ui::Parametres *ui;
    QSettings settings;
    QString CmBxToStr();

protected:
    void showEvent(QShowEvent *event) override;
};

#endif // PARAMETRES_H
