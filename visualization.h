#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QWidget>

namespace Ui {
class Visualization;
}

class Visualization : public QWidget
{
    Q_OBJECT

public:
    explicit Visualization(QWidget *parent = nullptr);
    ~Visualization();

private slots:
    //Main info receiver
    void get_data(QString, QString, QString);

    //Read information from local files
    void from_lf_readRoutes();
    void read_temporal();

    void setTable_1(QHash<QString, QString>);
    void setTable_2(QHash<QString, QString>);
    void setTable_3(QHash<QString, QString>);
    void setTable_4(QHash<QString, QString>);
    void showTime();

private:
    Ui::Visualization *ui;

    //Main information
    QString user;
    QString token;

    QHash<QString, QHash<QString,QString>>db_rutas;
    QHash<QString, QString>tabla_1;
    QHash<QString, QString>tabla_2;
    QHash<QString, QString>tabla_3;
    QHash<QString, QString>tabla_4;

   QHash<QString, QHash<QString,QString>>index_1;
   QHash<QString, QHash<QString,QString>>index_2;
   QHash<QString, QHash<QString,QString>>index_3;
   QHash<QString, QHash<QString,QString>>index_4;
};

#endif // VISUALIZATION_H
