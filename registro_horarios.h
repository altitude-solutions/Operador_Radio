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
    void pressSearchButton();
    void close_all();
    void send_update();

private slots:
    void save(QString);
    QString search_car(QString);
    void get_data(QString, QString, QString);
    QStringList eliminate_register(QString);
    void update_table(QHash<QString, QHash<QString,QString>>);
    void showTime();
    void read_temporal();
    void read_done();
    void set_data();
    void set_ruta();
    void set_conductor();
    void on_boton_registrar_clicked();
    void on_search_item_clicked();
    void on_close_button_clicked();
    void alarm_function(QString,QString);
    void on_table_gral_cellDoubleClicked(int row, int column);
    void on_pushButton_clicked();
    void on_butto_cancel_clicked();
    void on_button_update_clicked();
    void on_search_rbase_clicked();
    void on_search_iruta_clicked();
    void on_search_artua_clicked();
    void on_search_fruta_clicked();
    void on_search_irelleno_clicked();
    void on_search_srelleno_clicked();
    void on_search_ialmuerzo_clicked();
    void on_search_falmuerzo_clicked();
    void update_fields();
    void on_erase_rbase_clicked();
    void on_erase_iruta_clicked();
    void on_erase_aruta_clicked();
    void on_erase_fruta_clicked();
    void on_erase_irelleno_clicked();
    void on_erase_srelleno_clicked();
    void on_erase_ialmuerzo_clicked();
    void on_erase_falmuerzo_clicked();
    void on_pushButton_2_clicked();
    void recall(QString,QString);
    void on_table_gral_cellClicked(int row, int column);
    QString search_relation(QString, QString);
    void on_table_gral_cellChanged(int row, int column);
    void search_dependancy(QString);
    void saveJson(QHash<QString, QHash<QString,QString>>);
    QStringList search_same_id(QString, QHash<QString, QHash<QString,QString>>);

    void save_data();

    //Database Reading
    void from_db_readStaff();
    void from_db_readVehicles();
    void from_db_readLink_2();
    void from_db_readLink_1();
    void from_db_readRoutes();

    //Local data reading
    void from_lf_readStaff();
    void from_lf_readVehicles();
    void from_lf_readLink_2();
    void from_lf_readLink_1();
    void from_lf_readRoutes();

    void get_url(QString);
    void file_writing(QHash<QString, QHash<QString, QString>>, QString);

    void on_update_database_clicked();

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
    QHash<QString,QHash<QString,QString>>done;

    //This will change the table inbaase of the filtered value
    QHash<QString,QHash<QString,QString>>search_filter;

    //Qstring list for eliminating data
    QStringList eliminate_data;

    bool vehicle_exists;

    QStringList searching_completer;

    //This will save the global ID after double click in a table
    QString current_id;

    //This will deide what is shown in the table
    QString situation;

    //Goes next to current ID
    QString current_car;

    //This avoid to shoot the function everytime the table changes
    QString auxiliar_value;

    QString id_values;
    QString cycle_id;

    QString user_name, token;

    //This will change the table inbaase of the filtered value
    QHash<QString,QHash<QString,QString>>db_vehiculos;
    QHash<QString,QHash<QString,QString>>db_rutas;
    QHash<QString,QHash<QString,QString>>db_personal;
    QHash<QString,QHash<QString,QString>>db_link_VP;
    QHash<QString,QHash<QString,QString>>db_link_RV;

    QString url;

    QString global_session;


};

#endif // REGISTRO_HORARIOS_H
