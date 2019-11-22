#ifndef REGISTRO_PENALIDADES_H
#define REGISTRO_PENALIDADES_H

#include <QWidget>

namespace Ui {
class Registro_penalidades;
}

class Registro_penalidades : public QWidget
{
    Q_OBJECT

public:
    explicit Registro_penalidades(QWidget *parent = nullptr);
    ~Registro_penalidades();

private slots:
    void showTime();
    void get_data(QString);
    void set_description();

    void on_button_guardar_clicked();

private:
    Ui::Registro_penalidades *ui;
    QHash<QString,QHash<QString,QString>>penalidades;
    QHash<QString,QHash<QString,QString>>local_item;
    QHash<QString,QHash<QString,QString>>local_sigma;
    QHash<QString,QHash<QString,QString>>local_movil;
};

#endif // REGISTRO_PENALIDADES_H
