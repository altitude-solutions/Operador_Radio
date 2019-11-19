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

private:
    Ui::Registro_penalidades *ui;
};

#endif // REGISTRO_PENALIDADES_H
