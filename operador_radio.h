#ifndef OPERADOR_RADIO_H
#define OPERADOR_RADIO_H

#include <QMainWindow>
#include "registro_horarios.h"
#include "registro_penalidades.h"
#include "registro_datos.h"

namespace Ui {
class Operador_radio;
}

class Operador_radio : public QMainWindow
{
    Q_OBJECT

public:
    explicit Operador_radio(QWidget *parent = nullptr);
    ~Operador_radio();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void enviar_informacion(QString, QString, QString);
    void logOut();
    void send_url(QString);

private slots:
    void closer();
    void recibir_nombre(QString, QString, QString);
    void receive_url(QString);

private:
    Ui::Operador_radio *ui;
    Registro_horarios *registro_horarios;
    Registro_penalidades *registro_penalidades;
    Registro_datos *registro_datos;
    QString url;
};

#endif // OPERADOR_RADIO_H
