#ifndef REGISTRO_HORARIOS_H
#define REGISTRO_HORARIOS_H

#include <QWidget>

namespace Ui {
class Registro_horarios;
}

class Registro_horarios : public QWidget
{
    Q_OBJECT

public:
    explicit Registro_horarios(QWidget *parent = nullptr);
    ~Registro_horarios();

private slots:
    void showTime();
    void get_data(QString);
    void read_vehicles();
    void read_routes();
    void read_staff();
    void read_temporal();
    void update_table(QHash<QString, QHash<QString,QString>>);
    void set_data();
    void on_boton_registrar_clicked();
    void on_search_item_clicked();

    void on_button_add_clicked();

private:
    Ui::Registro_horarios *ui;

    //QHash to register the vehicle data, temporary local (read from a Json File) TODO::change the json with DataBase
    QHash<QString,QHash<QString,QString>>vehicles;

    //Hash to register the routes data, temporary local (read from a Json File) TODO::change the json with DataBase
    QHash<QString,QHash<QString,QString>>routes;

    //Hash to register the staff data, temporary local (read from a Json File) TODO::change the json with DataBase
    QHash<QString,QHash<QString,QString>>staff;

    //Temporal and un-finished data
    QHash<QString,QHash<QString,QString>>local_movil;

};

#endif // REGISTRO_HORARIOS_H
