#ifndef REGISTRO_PENALIDADES_H
#define REGISTRO_PENALIDADES_H

#include <QWidget>

namespace Ui {
class Registro_penalidades;
}

class Registro_penalidades : public QWidget
{
    Q_OBJECT

signals:
    void close();

public:

    explicit Registro_penalidades(QWidget *parent = nullptr);
    ~Registro_penalidades();
    void keyPressEvent(QKeyEvent *);

private slots:
    void showTime();
    void get_data(QString, QString, QString);
    void set_description();
    void on_button_guardar_clicked();
    void on_search_item_clicked();
    void on_search_sigma_clicked();
    void on_button_quitar_clicked();
    void on_table_gral_cellDoubleClicked(int row, int column);
    void on_button_respuesta_clicked();
    void on_butto_contrarespuesta_clicked();
    void save(QString);
    void update_table(QHash<QString,QHash<QString,QString>>);
    void read_routes();
    void read_vehicles();
    void read_staff();
    void on_close_button_clicked();
    bool search_existing(QString, QHash<QString,QHash<QString,QString>>);
    void on_button_update_clicked();
    void alarm_function(QString,QString,QString);
    void on_clean_clicked();
    void recall(QString,QString,QString);
    void on_anular_clicked();
    void on_table_gral_cellClicked(int row, int column);
    void on_supervisor_1_editingFinished();
    void get_url(QString);

    //Database Reading
    void from_db_readStaff();
    void from_db_readVehicles();
    void from_db_readLink_2();
    void from_db_readLink_1();
    void from_db_readRoutes();
    void from_db_readOverlords();

    void on_label_penalidad_editingFinished();

    void on_label_ruta_editingFinished();

    void on_label_movil_editingFinished();

private:
    Ui::Registro_penalidades *ui;

    //Variable for the Json reader
    QHash<QString,QHash<QString,QString>>penalidades;

    //Class variable for local data storage
    QHash<QString,QHash<QString,QString>>local_item;

    //Hash to register the routes data, temporary local (read from a Json File) TODO::change the json with DataBase
    QHash<QString,QHash<QString,QString>>routes;
    QHash<QString,QHash<QString,QString>>vehicles;
    QHash<QString,QHash<QString,QString>>staff;

    //Just in case for a dynamic filter
    QHash<QString,QHash<QString,QString>>sigma_filter;
    QHash<QString,QHash<QString,QString>>item_filter;

    //To work with the actual table
    QString actual_table;

    //locker to avoid Re-writing
    bool lock;

    //global id, to add new data
    QString actual_id;

    //Done data
    QHash<QString,QHash<QString,QString>>local_done;

    //Eliminate data just when close
    QStringList eliminate_data;

    QStringList supervisors;

    QString id_value;

    //UserName and Token
    QString user_name, token;
    QString url;

    //This will change the table inbaase of the filtered value
    QHash<QString,QHash<QString,QString>>db_vehiculos;
    QHash<QString,QHash<QString,QString>>db_rutas;
    QHash<QString,QHash<QString,QString>>db_personal;
    QHash<QString,QHash<QString,QString>>db_link_VP;
    QHash<QString,QHash<QString,QString>>db_link_RV;
    QHash<QString,QHash<QString,QString>>db_overlords;
};

#endif // REGISTRO_PENALIDADES_H
