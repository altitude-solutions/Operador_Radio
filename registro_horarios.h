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

signals:
    void logOut();

private slots:
    void save(QString);
    QString search_car(QString);
    void get_data(QString);
    void update_schedule(QHash<QString, QString>);
    QStringList eliminate_register(QString);
    void update_table(QHash<QString, QHash<QString,QString>>);
    void showTime();
    void read_vehicles();
    void read_routes();
    void read_staff();
    void read_temporal();
    void read_done();
    void set_data();
    void set_ruta();
    void set_conductor();
    void on_boton_registrar_clicked();
    void on_search_item_clicked();
    void on_button_add_clicked();
    void on_button_erase_clicked();
    void on_close_button_clicked();
    void alarm_function(QString);

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
    QHash<QString,QHash<QString,QString>>temporal;
    QHash<QString,QHash<QString,QString>>done;

    //Qstring list for eliminating data
    QStringList eliminate_data;

    bool vehicle_exists;
};

#endif // REGISTRO_HORARIOS_H
