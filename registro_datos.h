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
    void update_counter();
    void restart();

    void read_id();

    void on_restart_clicked();

    void on_table_gral_cellClicked(int row, int column);

    void on_button_cancel_clicked();

    void on_button_eliminate_clicked();

    void on_button_update_clicked();

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

};

#endif // REGISTRO_DATOS_H
