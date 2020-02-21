#include "visualization.h"
#include "ui_visualization.h"
#include <QScreen>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

Visualization::Visualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Visualization)
{
    ui->setupUi(this);

    //Get screen Size
   const auto screens = qApp->screens();

   int width = screens[0]->geometry().width();
   int height = screens[0]->geometry().height();

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*95)/1920;
    double pix_h = (height*95)/1080;
    QPixmap pix(":/images/img/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon->setFixedSize(static_cast<int>(pix_w), static_cast<int>(pix_h));

    ui -> frame_6 -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.75));

    //Setting the tables
    ui -> table_1 -> setColumnCount(1);
    for(int r=0; r<1; r++){
        ui->table_1 -> setColumnWidth(r,static_cast<int>(width/4));
        ui->table_1 -> setRowHeight(r,static_cast<int>(height/15));
    }

    ui -> table_2 -> setColumnCount(1);
    for(int r=0; r<1; r++){
        ui->table_2 -> setColumnWidth(r,static_cast<int>(width/4));
        ui->table_2 -> setRowHeight(r,static_cast<int>(height/15));
    }

    ui -> table_3 -> setColumnCount(1);
    for(int r=0; r<1; r++){
        ui->table_3 -> setColumnWidth(r,static_cast<int>(width/4));
        ui->table_3 -> setRowHeight(r,static_cast<int>(height/15));
    }

    ui -> table_4 -> setColumnCount(1);
    for(int r=0; r<1; r++){
        ui->table_4 -> setColumnWidth(r,static_cast<int>(width/4));
        ui->table_4 -> setRowHeight(r,static_cast<int>(height/15));
    }

    connect(ui-> pushButton, SIGNAL(clicked()),this,SLOT(from_lf_readRoutes()));
}

Visualization::~Visualization(){
    delete ui;
}

void Visualization::get_data(QString real_name, QString user_name, QString token){

    ui -> label_user ->setText(real_name);
    this -> user = user_name;
    this -> token = token;

    from_lf_readRoutes();
}

void Visualization::from_lf_readRoutes(){

    db_rutas.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/db_files/rutas.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = document.array();

    foreach(QJsonValue object, arraydatos){

        QHash<QString,QString> current;
        current.insert ("id", object.toObject ().value ("id").toString());
        current.insert ("ruta", object.toObject ().value ("ruta").toString());
        current.insert("tipo",object.toObject().value("tipoVehiculos").toString());
        db_rutas.insert(object.toObject ().value("id").toString(), current);
    }
    read_temporal();
}

void Visualization::read_temporal(){

    tabla_1.clear();
    tabla_2.clear();
    tabla_3.clear();
    tabla_4.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/pendant_horarios.txt";
    QFile file(filename );
    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }
    QJsonDocument documentyd = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = documentyd.array();

    foreach(QJsonValue objetoxd, arraydatos){
        if(objetoxd.toObject().value("id").toString()!=""){
            QString id_ruta = objetoxd.toObject().value("ruta_id").toString();

            if(db_rutas[id_ruta]["tipo"]=="1"){
                tabla_1[objetoxd.toObject().value("ruta").toString()] = objetoxd.toObject().value("ruta_id").toString();
            }
            else if(db_rutas[id_ruta]["tipo"]=="4"){
                tabla_2[objetoxd.toObject().value("ruta").toString()] = objetoxd.toObject().value("ruta_id").toString();
            }
            else if(db_rutas[id_ruta]["tipo"]=="5"){
                tabla_3[objetoxd.toObject().value("ruta").toString()] = objetoxd.toObject().value("ruta_id").toString();
            }
            else {
                tabla_4[objetoxd.toObject().value("ruta").toString()] = objetoxd.toObject().value("ruta_id").toString();
            }
        }
    }
    setTable_1(tabla_1);
    setTable_2(tabla_2);
    setTable_3(tabla_3);
    setTable_4(tabla_4);
}

void Visualization::setTable_1(QHash<QString,QString> update){
    //Rewrite the local table
    ui -> table_1 -> setRowCount(0);

    QHashIterator<QString, QHash<QString, QString>>rutes(db_rutas);

    QStringList rutas_1;

    while(rutes.hasNext()){
        auto key = rutes.next().key();
        if(db_rutas[key]["tipo"]=="1"){
            rutas_1<<db_rutas[key]["ruta"];
        }
    }

    ui->table_1->setSortingEnabled(false);
    foreach (QString item, rutas_1) {
        int  row_control;
        ui->table_1->insertRow(ui->table_1->rowCount());
        row_control= ui->table_1->rowCount()-1;

        //Writing the current row
        ui->table_1->setItem(row_control, 0, new QTableWidgetItem(item));

        if(update[item]!=""){
            ui->table_1->item(row_control,0)->setBackground(QColor("#B7E1DF"));
        }
        else{
            ui->table_1->item(row_control,0)->setBackground(QColor("white"));
        }
    }

    ui -> table_1->setSortingEnabled(true);
}

void Visualization::setTable_2(QHash<QString, QString>update){

//Rewrite the local table
    ui -> table_2 -> setRowCount(0);

    QHashIterator<QString, QHash<QString, QString>>rutes(db_rutas);

    QStringList rutas_2;

    while(rutes.hasNext()){
        auto key = rutes.next().key();
        if(db_rutas[key]["tipo"]=="4"){
            rutas_2<<db_rutas[key]["ruta"];
        }
    }

    ui->table_2->setSortingEnabled(false);
    foreach (QString item, rutas_2) {
        int  row_control;
        ui->table_2->insertRow(ui->table_2->rowCount());
        row_control= ui->table_2->rowCount()-1;

        //Writing the current row
        ui->table_2->setItem(row_control, 0, new QTableWidgetItem(item));

        if(update[item]!=""){
            ui->table_2->item(row_control,0)->setBackground(QColor("#B7E1DF"));
        }
        else{
            ui->table_2->item(row_control,0)->setBackground(QColor("white"));
        }
    }

    ui -> table_2->setSortingEnabled(true);
}

void Visualization::setTable_3(QHash<QString, QString>update){

    //Rewrite the local table
        ui -> table_3 -> setRowCount(0);

        QHashIterator<QString, QHash<QString, QString>>rutes(db_rutas);

        QStringList rutas_3;

        while(rutes.hasNext()){
            auto key = rutes.next().key();
            if(db_rutas[key]["tipo"]=="5"){
                rutas_3<<db_rutas[key]["ruta"];
            }
        }

        ui->table_3->setSortingEnabled(false);
        foreach (QString item, rutas_3) {
            int  row_control;
            ui->table_3->insertRow(ui->table_3->rowCount());
            row_control= ui->table_3->rowCount()-1;

            //Writing the current row
            ui->table_3->setItem(row_control, 0, new QTableWidgetItem(item));

            if(update[item]!=""){
                ui->table_3->item(row_control,0)->setBackground(QColor("#B7E1DF"));
            }
            else{
                ui->table_3->item(row_control,0)->setBackground(QColor("white"));
            }
        }

        ui -> table_3->setSortingEnabled(true);
}

void Visualization::setTable_4(QHash<QString, QString>update){
    //Rewrite the local table
        ui -> table_4 -> setRowCount(0);

        QHashIterator<QString, QHash<QString, QString>>rutes(db_rutas);

        QStringList rutas_4;

        while(rutes.hasNext()){
            auto key = rutes.next().key();
            if(db_rutas[key]["tipo"]!="1"&&db_rutas[key]["tipo"]!="4"&&db_rutas[key]["tipo"]!="5"){
                rutas_4<<db_rutas[key]["ruta"];
            }
        }

        ui->table_4->setSortingEnabled(false);
        foreach (QString item, rutas_4) {
            int  row_control;
            ui->table_4->insertRow(ui->table_4->rowCount());
            row_control= ui->table_4->rowCount()-1;

            //Writing the current row
            ui->table_4->setItem(row_control, 0, new QTableWidgetItem(item));

            if(update[item]!=""){
                ui->table_4->item(row_control,0)->setBackground(QColor("#B7E1DF"));
            }
            else{
                ui->table_4->item(row_control,0)->setBackground(QColor("white"));
            }
        }

        ui -> table_4->setSortingEnabled(true);
}

