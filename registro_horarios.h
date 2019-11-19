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

private:
    Ui::Registro_horarios *ui;
};

#endif // REGISTRO_HORARIOS_H
