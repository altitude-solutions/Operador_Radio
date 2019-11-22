#include "registro_penalidades.h"
#include "ui_registro_penalidades.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

Registro_penalidades::Registro_penalidades(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_penalidades)
{
    ui->setupUi(this);
    //Get screen Size
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*120)/1920;
    double pix_h = (height*120)/1080;
    QPixmap pix(":/images/img/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon->setFixedSize(static_cast<int>(pix_w), static_cast<int>(pix_h));

    //Set the table Size
    ui ->table_gral ->setColumnCount(12);
    for(int r=0; r<12; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/12.71));
    }

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.13));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.25));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.4));

    //Set Search icons
    QPixmap pix_b1(":/images/img/search_2.png");
    QIcon ButtonIcon(pix_b1);
    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));
    ui->search_sigma->setIcon(ButtonIcon);
    ui->search_sigma->setIconSize(QSize(20,20));

    //Setting the table headers
    QStringList headers = {"Item",
                           "tipo de penalidad",
                           "Sigma",
                           "Ruta",
                           "Movil",
                           "Detalle",
                           "Hora recepción",
                           "Supervisor",
                           "Respuesta",
                           "Hora Respuesta",
                           "Contra Respuesta",
                           "Hora contrarespuesta"};

    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    //Read the JSon File
    QString contenido;
    QString filename= ":/resources/Recursos/penalidades.txt";
    QFile file(filename );
    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    //Save A Hash from th Json File
    QJsonDocument doc = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray array_datos = doc.array();

    foreach(QJsonValue object, array_datos){
        QHash<QString,QString> current;
        current.insert("Detalle", object.toObject().value("Detalle").toString());
        current.insert("Tipo", object.toObject().value("Tipo").toString());
        current.insert("Unidad",object.toObject().value("Unidad").toString());
        current.insert("Puntos",QString::number(object.toObject().value("Puntos").toInt()));
        current.insert("Clasificacion",object.toObject().value("Clasificacion").toString());

        penalidades.insert(QString::number(object.toObject().value("Item").toInt()),current);
     }

    //connect between the item and the description
    connect(ui->label_item,SIGNAL(editingFinished()),this,SLOT(set_description()));

    //connect between the lines and the save button
    connect(ui->label_sigma, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_penalidad, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_movil, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_ruta, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_detalle, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
}

Registro_penalidades::~Registro_penalidades()
{
    delete ui;
}

void Registro_penalidades::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
    ui->label_date->setText(tiempo);
}

void Registro_penalidades::get_data(QString username){
    ui->label_user->setText(username);
}

void Registro_penalidades::set_description(){
    QString actual_item = ui->label_item->text();
    if(penalidades[actual_item]["Detalle"]!=""){
        ui -> label_description -> setText(penalidades[actual_item]["Detalle"]);
        ui -> label_penalidad -> setText(penalidades[actual_item]["Tipo"]);
    }
    else{
        QMessageBox::critical(this,"data","Item Inexistente");
    }
}

void Registro_penalidades::on_button_guardar_clicked()
{
    //This button sends the data to the local Hash and shows it in the table
    QString sigma = ui -> label_sigma -> text();
    QString tipo = ui -> label_penalidad -> text();
    QString ruta = ui -> label_ruta -> text();
    QString movil = ui -> label_movil -> text();
    QString item = ui -> label_item -> text();
    QString detalle = ui -> label_detalle -> text();
    QString recepcion = ui -> label_date -> text();

    if(sigma!="" && tipo!="" && ruta!="" && movil!="" && item!=""){
        local_sigma[sigma]["item"] = item;
        local_sigma[sigma]["tipo"] = tipo;
        local_sigma[sigma]["ruta"] = ruta;
        local_sigma[sigma]["movil"] = movil;
        local_sigma[sigma]["detalle"] = detalle;
        local_sigma[sigma]["recepcion"] = recepcion;

        local_movil[movil]["sigma"] = sigma;
        local_movil[movil]["tipo"] = tipo;
        local_movil[movil]["ruta"] = ruta;
        local_movil[movil]["item"] = item;
        local_movil[movil]["detalle"] = detalle;
        local_movil[movil]["recepcion"] = recepcion;

        local_item[item]["sigma"] = sigma;
        local_item[item]["tipo"] = tipo;
        local_item[item]["ruta"] = ruta;
        local_item[item]["movil"] = movil;
        local_item[item]["detalle"] = detalle;
        local_item[item]["recepcion"] = recepcion;

        ui -> table_gral -> setRowCount(0);
        //Writing the table only with the data saved (avoid overwritting)
        QHashIterator<QString, QHash<QString, QString>>iter(local_sigma);
        while(iter.hasNext()){

            auto current = iter.next().key();

            //Add a new row
            int  row_control;
            ui->table_gral->insertRow(ui->table_gral->rowCount());
            row_control= ui->table_gral->rowCount()-1;

            //Writing the current row
            ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(local_sigma[current]["item"]));
            ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(local_sigma[current]["tipo"]));
            ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(current));
            ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(local_sigma[current]["ruta"]));
            ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(local_sigma[current]["movil"]));
            ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(local_sigma[current]["detalle"]));
            ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(local_sigma[current]["recepcion"]));
        }
    }
    else{
        QMessageBox::critical(this,"data","Campos incompletos");
    }
}
