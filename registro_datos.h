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
    void keyPressEvent(QKeyEvent *);

signals:
    void logOut();
    void close_all();
    void send_update();

private slots:
    void showTime();
    void get_data(QString, QString, QString);
    QString search_dato(QString);
    void enable();
    void on_button_guardar_clicked();
    void read_temporal();
    void save(QString);
    void save_counter(int);
    void read_counter();
    void update_table(QHash<QString, QHash<QString,QString>>);
    void read_done();
    void on_search_item_clicked();
    void on_button_respuesta_clicked();
    void on_button_respuesta_2_clicked();
    void on_button_respuesta_4_clicked();
    void on_button_respuesta_3_clicked();
    void on_close_button_clicked();
    void restart();
    void on_restart_clicked();
    void on_table_gral_cellClicked(int row, int column);
    void on_button_cancel_clicked();
    void on_button_eliminate_clicked();
    void on_button_update_clicked();
    void get_url(QString);

    //Database Reading
    void from_db_readOverlords();
    void from_db_readDatos();

    //Local File reading
    void from_lf_readStaff();
    void from_lf_readOverlords();
    void from_lf_readDatos();

    //Update to a local File
    void update_data();

    void saveJson(QHash<QString,QHash<QString,QString>>);
    void on_comunicacion_editingFinished();
    void on_ejecucion_editingFinished();
    void on_verificacion_editingFinished();
    void save_data();

    void file_writing(QHash<QString, QHash<QString, QString>>, QString);

private:
    Ui::Registro_datos *ui;
    QString auxiliar;

    //Temporal and un-finished data
    QHash<QString,QHash<QString,QString>>temporal;
    QHash<QString,QHash<QString,QString>>done;

    //Counter for the id
    int counter;

    //To eliminate at the end
    QStringList eliminate_list;

    //Past Time variable
    int past;

    //Control the buttons state
    bool lock;
    bool lock_b;

    QString current_id;
    QString id_value;

    QString user_name, token;
    QString url;

    QHash<QString,QHash<QString,QString>>db_personal;
    QHash<QString,QHash<QString,QString>>db_overlords;
    QHash<QString,QHash<QString,QString>>db_datos;

    QString global_session;

};

#endif // REGISTRO_DATOS_H
