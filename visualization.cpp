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

    //Setting the tables
    ui -> table_1 -> setColumnCount(3);
    for(int r=0; r<3; r++){
        ui->table_1 -> setColumnWidth(r,static_cast<int>(width/6));
        ui->table_1 -> setRowHeight(r,static_cast<int>(height/15));
    }
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
                tabla_1[objetoxd.toObject().value("ruta_id").toString()] = objetoxd.toObject().value("ruta").toString();
            }
            else if(db_rutas[id_ruta]["tipo"]=="4"){
                tabla_2[objetoxd.toObject().value("ruta_id").toString()] = objetoxd.toObject().value("ruta").toString();
            }
            else if(db_rutas[id_ruta]["tipo"]=="5"){
                tabla_3[objetoxd.toObject().value("ruta_id").toString()] = objetoxd.toObject().value("ruta").toString();
            }
            else {
                tabla_4[objetoxd.toObject().value("ruta_id").toString()] = objetoxd.toObject().value("ruta").toString();
            }
        }
    }
    setTable_1(tabla_1);
}

void Visualization::setTable_1(QHash<QString,QString> update){
    //Rewrite the local table
    ui -> table_1 -> setRowCount(0);

    QHashIterator<QString, QString>iter(update);

    ui->table_1->setSortingEnabled(false);

    int size = update.size();
    qDebug()<<size;


//        //Add a new row
//        int  row_control;
//        ui->table_1->insertRow(ui->table_1->rowCount());
//        row_control= ui->table_1->rowCount()-1;

//        //Writing the current row
//        ui->table_1->setItem(row_control, 0, new QTableWidgetItem(update[current]));
//        ui->table_1->setItem(row_control, 1, new QTableWidgetItem(update[current]));
//        ui->table_1->setItem(row_control, 2, new QTableWidgetItem(update[current]));
//    }
    //    ui->table_1->setSortingEnabled(true);
}

void Visualization::setTable_2(QHash<QString, QString>)
{

}

void Visualization::setTable_3(QHash<QString, QString>)
{

}

void Visualization::setTable_4(QHash<QString, QString>)
{

}
