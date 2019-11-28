#ifndef REGISTRO_DATOS_H
#define REGISTRO_DATOS_H

#include <QWidget>

namespace Ui {
class Registro_datos;
}

class Registro_datos : public QWidget
{
    Q_OBJECT

public:
    explicit Registro_datos(QWidget *parent = nullptr);
    ~Registro_datos();

private slots:
    void showTime();
    void get_data(QString);
    void enable();
    void on_button_guardar_clicked();

private:
    Ui::Registro_datos *ui;
    QString auxiliar;
};

#endif // REGISTRO_DATOS_H
