#include "registro_horarios.h"
#include "ui_registro_horarios.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QMessageBox>

Registro_horarios::Registro_horarios(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_horarios)
{
    ui->setupUi(this);

    //Get screen Size
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*95)/1920;
    double pix_h = (height*95)/1080;
    QPixmap pix(":/images/img/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon->setFixedSize(static_cast<int>(pix_w), static_cast<int>(pix_h));

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.12));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.25));
    ui -> frame_9 -> setFixedHeight(static_cast<int>(height*0.4));

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    //Set the table Size
    ui ->table_gral ->setColumnCount(13);
    for(int r=0; r<13; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/13.71));
    }

    //Setting the table headers
    QStringList headers = {"Móvil",
                           "Ruta",
                           "Conductor",
                           "Ayudantes",
                           "Salida Base",
                           "Inicio ruta",
                           "Final ruta",
                           "Abandono ruta",
                           "Ingreso relleno",
                           "Salida relleno",
                           "Inicio Almuerzo",
                           "Final almuerzo",
                           "Regreso a base"};

    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    //Setting the Scroll
    QStringList scroll_items = { "11 - Inicio Ruta",
                           "12 - Final Ruta",
                           "13 - Abandono Ruta",
                           "21 - Ingreso Relleno Sanitario",
                           "22 - Salida Relleno Sanitario",
                           "31 - Inicio Almuerzo",
                           "32- Final Almuerzo",
                           "99 - Regreso base"};

    foreach (QString itm, scroll_items){
            ui -> selector -> addItem(itm);
     }

    ui -> selector-> setEditable(true);
    ui -> selector-> setCurrentIndex(-1);
    ui -> selector-> setCurrentText("Seleccionar item");

    //Read all Data
    read_vehicles();
    read_routes();
    read_staff();
    read_temporal();

    //Set icons
    double icon_w = (width*120)/1920;
    double icon_h = (height*120)/1080;

    QPixmap icon_movil(":/images/img/movil.png");
    ui->icon_movil->setPixmap(icon_movil.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_movil->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_ruta(":/images/img/ruta.png");
    ui->icon_ruta->setPixmap(icon_ruta.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ruta->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_conductor(":/images/img/conductor.png");
    ui->icon_conductor->setPixmap(icon_conductor.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_conductor->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_ayudantes(":/images/img/ayudantes.png");
    ui->icon_ayudantes->setPixmap(icon_ayudantes.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ayudantes->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    //Set Search icons
    QPixmap pix_b1(":/images/img/search_2.png");
    QIcon ButtonIcon(pix_b1);
    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////CONNECTIONS////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    //connect between the item and the description
    connect(ui->label_movil,SIGNAL(editingFinished()),this,SLOT(set_data()));

    //Connections between the text edit and the register button
    connect(ui->label_ayudantes, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));
    connect(ui->label_conductor, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));
    connect(ui->label_ruta, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));

    //connect the searchline with the search button
    connect(ui->label_search, SIGNAL(returnPressed()),ui->search_item, SLOT(click()));

}

    Registro_horarios::~Registro_horarios()
{
    delete ui;
}

void Registro_horarios::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
    ui->label_date->setText(tiempo);
}

void Registro_horarios::get_data(QString username){
    ui->label_user->setText(username);
}

void Registro_horarios::set_data(){

    QString actual_item = ui->label_movil->text();
    if(vehicles[actual_item]["movil"]!=""){
        //TODO Add a condition here for changing the route in case of the time
        ui -> label_ruta -> setText(vehicles[actual_item]["ruta"]);
        //TODO switch betwen the driver one and two
        ui -> label_conductor -> setText(staff[vehicles[actual_item]["conductor"]]["nombre"]);
        ui -> label_ayudantes -> setText(vehicles[actual_item]["numeroDeAyudantes"]);
    }
    else{
        ui->label_movil->setText("");
        QMessageBox::critical(this,"data","Vehículo no se encuentra registrado en la base de datos");
    }
}

void Registro_horarios::read_vehicles(){

    //Read the JSon File
    QString contenido;
    QString filename= ":/resources/Recursos/vehicles.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    //Save A Hash from the Json File
    QJsonDocument doc = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray array_datos = doc.array();

    foreach(QJsonValue object, array_datos){
        QHash<QString,QString> current;
        current.insert("movil", object.toObject().value("movil").toString());
        current.insert("placa", object.toObject().value("placa").toString());
        current.insert("tipoDeVehiculo",object.toObject().value("tipoDeVehiculo").toString());
        current.insert("servicios",object.toObject().value("servicios").toString());
        current.insert("codTipoDeVehiculo",object.toObject().value("codTipoDeVehiculo").toString());
        current.insert("descripcion",object.toObject().value("descripcion").toString());
        current.insert("cargaToneladas",object.toObject().value("cargaToneladas").toString());
        current.insert("cargaMetrosCubicos",object.toObject().value("cargaMetrosCubicos").toString());
        current.insert("cargaLitros",object.toObject().value("cargaLitros").toString());
        current.insert("marca",object.toObject().value("marca").toString());
        current.insert("modelo",object.toObject().value("modelo").toString());
        current.insert("version",object.toObject().value("version").toString());
        current.insert("anio",object.toObject().value("anio").toString());
        current.insert("cilindrada",object.toObject().value("cilindrada").toString());
        current.insert("traccion",object.toObject().value("traccion").toString());
        current.insert("peso",object.toObject().value("peso").toString());
        current.insert("combustible",object.toObject().value("combustible").toString());
        current.insert("ruedas",object.toObject().value("ruedas").toString());
        current.insert("motor",object.toObject().value("motor").toString());
        current.insert("turbo",object.toObject().value("turbo").toString());
        current.insert("chasis",object.toObject().value("chasis").toString());
        current.insert("serie",object.toObject().value("serie").toString());
        current.insert("color",object.toObject().value("color").toString());
        current.insert("conductor",object.toObject().value("conductor").toString());
        current.insert("conductor_2",object.toObject().value("conductor_2").toString());
        current.insert("numeroDeAyudantes",object.toObject().value("numeroDeAyudantes").toString());
        current.insert("ruta",object.toObject().value("ruta").toString());
        current.insert("ruta_2",object.toObject().value("ruta_2").toString());
        current.insert("proyecto",object.toObject().value("proyecto").toString());

        vehicles.insert(object.toObject().value("movil").toString(),current);
     }
}

void Registro_horarios::read_routes(){
    //Read the JSon File
    QString contenido;
    QString filename= ":/resources/Recursos/rutas.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    //Save A Hash from the Json File
    QJsonDocument doc = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray array_datos = doc.array();

    foreach(QJsonValue object, array_datos){
        QHash<QString,QString> current;
        current.insert("ruta", object.toObject().value("ruta").toString());
        current.insert("servicio", object.toObject().value("servicio").toString());
        current.insert("tipoDeVehiculos",object.toObject().value("tipoDeVehiculos").toString());
        current.insert("referencia",object.toObject().value("referencia").toString());
        current.insert("zona",object.toObject().value("zona").toString());
        current.insert("turno",object.toObject().value("turno").toString());
        current.insert("frecuencia",object.toObject().value("frecuencia").toString());
        current.insert("subServicio",object.toObject().value("subServicio").toString());

        routes.insert(object.toObject().value("ruta").toString(),current);
     }
}

void Registro_horarios::read_staff(){
    //Read the JSon File
    QString contenido;
    QString filename= ":/resources/Recursos/staff.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    //Save A Hash from the Json File
    QJsonDocument doc = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray array_datos = doc.array();

    foreach(QJsonValue object, array_datos){
        QHash<QString,QString> current;
        current.insert("idPersonal", object.toObject().value("idPersonal").toString());
        current.insert("nombre", object.toObject().value("nombre").toString());
        current.insert("carnet",object.toObject().value("carnet").toString());
        current.insert("cargo",object.toObject().value("cargo").toString());
        current.insert("proyecto",object.toObject().value("proyecto").toString());
        current.insert("turno",object.toObject().value("turno").toString());
        current.insert("zona",object.toObject().value("zona").toString());
        current.insert("subZona",object.toObject().value("subZona").toString());
        current.insert("supervisor",object.toObject().value("supervisor").toString());
        current.insert("diasLaborales",object.toObject().value("diasLaborales").toString());

        staff.insert(object.toObject().value("idPersonal").toString(),current);
     }
}

void Registro_horarios::read_temporal(){

    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/temporal_data.txt";
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
        QHash<QString,QString> current;
        current.insert("00 - Salida base", objetoxd.toObject().value("00 - Salida base").toString());
        current.insert("11 - Inicio Ruta", objetoxd.toObject().value("11 - Inicio Ruta").toString());
        current.insert("12 - Final Ruta", objetoxd.toObject().value("12 - Final Ruta").toString());
        current.insert("13 - Abandono Ruta", objetoxd.toObject().value("13 - Abandono Ruta").toString());
        current.insert("21 - Ingreso Relleno Sanitario",objetoxd.toObject().value("21 - Ingreso Relleno Sanitario").toString());
        current.insert("22 - Salida Relleno Sanitario",objetoxd.toObject().value("22 - Salida Relleno Sanitario").toString());
        current.insert("31 - Inicio Almuerzo",objetoxd.toObject().value("31 - Inicio Almuerzo").toString());
        current.insert("32- Final Almuerzo",objetoxd.toObject().value("32- Final Almuerzo").toString());
        current.insert("99 - Regreso base",objetoxd.toObject().value("99 - Regreso base").toString());

        current.insert("ruta",objetoxd.toObject().value("ruta").toString());
        current.insert("numeroDeAyudantes",objetoxd.toObject().value("numeroDeAyudantes").toString());
        current.insert("conductor",objetoxd.toObject().value("conductor").toString());

        local_movil.insert(objetoxd.toObject().value("movil").toString(),current);
    }
}

void Registro_horarios::update_table(QHash<QString, QHash<QString,QString>>update){

    //Rewrite the local table
    ui -> table_gral -> setRowCount(0);

    QHashIterator<QString, QHash<QString, QString>>iter(update);

    while(iter.hasNext()){

        auto current = iter.next().key();

        //Add a new row
        int  row_control;
        ui->table_gral->insertRow(ui->table_gral->rowCount());
        row_control= ui->table_gral->rowCount()-1;

        //Writing the current row
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(current));
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["ruta"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["conductor"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["ayudantes"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["salida_base"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["Inicio_ruta"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["Final_ruta"]));
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["Abandono_ruta"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["Ingreso_relleno"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(update[current]["Salida_relleno"]));
        ui->table_gral->setItem(row_control, 10, new QTableWidgetItem(update[current]["Inicio_almuerzo"]));
        ui->table_gral->setItem(row_control, 11, new QTableWidgetItem(update[current]["Final_almuerzo"]));
        ui->table_gral->setItem(row_control, 12, new QTableWidgetItem(update[current]["Regreso_base"]));
    }
}

void Registro_horarios::on_boton_registrar_clicked()
{
    QString movil = ui -> label_movil -> text();
    QString ruta = ui -> label_ruta -> text();
    QString conductor = ui -> label_conductor -> text();
    QString ayudantes = ui -> label_ayudantes -> text();
    QString time = ui -> label_date -> text();


    if (local_movil[movil]["salida_base"] ==""){

        local_movil[movil]["ruta"]=ruta;
        local_movil[movil]["conductor"]=conductor;
        local_movil[movil]["ayudantes"]=ayudantes;
        local_movil[movil]["salida_base"]=time  ;

        update_table(local_movil);

        //Restart the values from every text edit
        ui -> label_movil -> setText("");
        ui -> label_ruta -> setText("");
        ui -> label_conductor -> setText("");
        ui -> label_ayudantes -> setText("");
    }
    else{
        QMessageBox::critical(this,"data","Este vehículo todavía no concluyó con su ciclo");
    }
}

void Registro_horarios::on_search_item_clicked()
{
    QString search = ui -> label_search -> text();

    int counter = 0;
    //Search for the register in the table
    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);

    while(iter.hasNext()){
        auto current = iter.next().key();
        if (current==search){
            counter = 1;
            break;
        }
    }

    if(counter == 1){
        ui -> frame_movil -> setText(search);
        ui -> frame_ruta -> setText(local_movil[search]["ruta"]);
        ui -> frame_conductor -> setText(local_movil[search]["conductor"]);
        ui -> frame_ayudantes -> setText(local_movil[search]["ayudantes"]);

        //Restart the action selector TODO - Suggest the next action
        ui -> selector-> setEditable(true);
        ui -> selector-> setCurrentIndex(-1);
        ui -> selector-> setCurrentText("Seleccionar item");
    }
    else{
         QMessageBox::critical(this,"data","No se registró una salida de base para este vehículo");
    }
}

void Registro_horarios::on_button_add_clicked()
{
    QString action =ui -> selector -> currentText();
    QString actual = ui -> label_search -> text();
    QString time = ui -> label_date -> text();

    //TODO ADD A OPTION TO AVOID OVERWRITTING
    if(action!="Seleccionar item"){
        if(actual!=""){
            if(action == "11 - Inicio Ruta"){
                local_movil[actual]["Inicio_ruta"] = time;
            }
            else if(action ==  "12 - Final Ruta"){
                local_movil[actual]["Final_ruta"] = time;
            }
            else if (action == "13 - Abandono Ruta"){
                local_movil[actual]["Abandono_ruta"] = time;
            }
            else if (action == "21 - Ingreso Relleno Sanitario"){
                local_movil[actual]["Ingreso_relleno"] = time;
            }
            else if(action ==  "22 - Salida Relleno Sanitario"){
                local_movil[actual]["Salida_relleno"] = time;
            }
            else if(action == "31 - Inicio Almuerzo"){
                local_movil[actual]["Inicio_almuerzo"] = time;
            }
            else if (action == "32- Final Almuerzo"){
                local_movil[actual]["Final_almuerzo"] = time;
            }
            else if(action == "99 - Regreso base"){
                //In this case we have to close the cycle
                local_movil[actual]["Regreso_base"] = time;
            }
            update_table(local_movil);
        }
        else{
            QMessageBox::critical(this,"data","Seleccionar un vehículo porfavor");
        }
    }
    else{
         QMessageBox::critical(this,"data","Seleccionar una acción a realizar porfavor");
    }
}
