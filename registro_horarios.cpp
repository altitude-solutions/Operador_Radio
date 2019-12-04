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
#include <QCompleter>
#include <QStringListModel>
#include <QCloseEvent>
#include <QScreen>
#include <QtAlgorithms>

Registro_horarios::Registro_horarios(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_horarios)
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
    ui -> table_gral -> setColumnCount(13);
    ui -> table_horarios -> setColumnCount(2);
    for(int r=0; r<13; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/13.71));
    }
    for(int r=0; r<2; r++){
        ui->table_horarios ->setColumnWidth(r,static_cast<int>(width/15.5));
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
                           "99 - Regreso Base"};

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
    read_done();

    update_table(local_movil);

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
    connect(ui->label_ruta,SIGNAL(editingFinished()),this,SLOT(set_ruta()));
    connect(ui->label_conductor,SIGNAL(editingFinished()),this,SLOT(set_conductor()));
    connect(ui->label_search,SIGNAL(editingFinished()),ui->search_item,SLOT(click()));

    //Connections between the text edit and the register button
    connect(ui->label_ayudantes, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));

    //connect(ui->label_conductor, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));
    //connect(ui->label_ruta, SIGNAL(returnPressed()),ui->boton_registrar, SLOT(click()));

    //connect the searchline with the search button
    connect(ui->label_search, SIGNAL(returnPressed()),ui->search_item, SLOT(click()));

    update_schedule(vehicles["0"]);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////////COMPLETERS////////////////////////////
    /////////////////////////////////////////////////////////////////////

    //Extracting labels for movil
    QHashIterator<QString, QHash<QString, QString>>movil_iter(vehicles);
    QStringList movil_list;

    while(movil_iter.hasNext()){
        movil_list<<movil_iter.next().key();
    }
    std::sort(movil_list.begin(), movil_list.end());

    QCompleter *movil_completer = new QCompleter(movil_list,this);

    movil_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    movil_completer -> setCompletionMode(QCompleter::PopupCompletion);
    movil_completer -> setFilterMode(Qt::MatchStartsWith);
    ui -> label_movil -> setCompleter(movil_completer);


    //Extracting labels for routes
    QHashIterator<QString, QHash<QString, QString>>routes_iter(routes);
    QStringList routes_list;

    while(routes_iter.hasNext()){
        routes_list<<routes_iter.next().key();
    }
    QCompleter *routes_completer = new QCompleter(routes_list,this);

    routes_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    routes_completer -> setCompletionMode(QCompleter::PopupCompletion);
    routes_completer -> setFilterMode(Qt::MatchContains);
    ui -> label_ruta -> setCompleter(routes_completer);

    //Extracting labels for staff
    QHashIterator<QString, QHash<QString, QString>>staff_iter(staff);
    QStringList staff_list;

    while(staff_iter.hasNext()){
        staff_list<<staff[staff_iter.next().key()]["nombre"];
    }
    QCompleter *staff_completer = new QCompleter(staff_list,this);

    staff_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    staff_completer -> setCompletionMode(QCompleter::PopupCompletion);
    staff_completer -> setFilterMode(Qt::MatchContains);
    ui -> label_conductor -> setCompleter(staff_completer);

    //Set a completer for the search button
    QHashIterator<QString, QHash<QString, QString>>search_iter(local_movil);
    QStringList searching;
    QString random;
    while(search_iter.hasNext()){
        random = local_movil[search_iter.next().key()]["movil"];
        searching << random;
        qDebug()<<random;
    }

    std::sort(searching.begin(), searching.end());

    QCompleter *search_completer = new QCompleter(searching,this);

    search_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    search_completer -> setCompletionMode(QCompleter::PopupCompletion);
    search_completer -> setFilterMode(Qt::MatchStartsWith);
    ui -> label_search -> setCompleter(search_completer);

    //Initialize the eliminate data
    eliminate_data.clear();

    //set the vehicle exists by default
    vehicle_exists = false;
}

    Registro_horarios::~Registro_horarios()
{
    delete ui;
}

void Registro_horarios::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh/mm/ss");
    ui->label_date->setText(tiempo);
}

void Registro_horarios::get_data(QString username){
    ui->label_user->setText(username);
}

void Registro_horarios::set_data(){

    QString actual_item = ui->label_movil->text();
    if(actual_item!=""){
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
}

void Registro_horarios::set_ruta(){

    QString actual_item = ui->label_ruta->text();
    if(actual_item!=""){
        if(routes[actual_item]["ruta"]==""){
            ui->label_ruta->setText("");
            QMessageBox::critical(this,"data","Ruta Inválida");
        }
    }
}

void Registro_horarios::set_conductor(){

    QString actual_item = ui->label_conductor->text();
    int counter = 0;
    if(actual_item!=""){
        QHashIterator<QString, QHash<QString, QString>>iter(staff);
        while(iter.hasNext()){
            auto current = iter.next().key();
            if(staff[current]["nombre"]==actual_item){
                counter=1;
                break;
            }
        }
        if(counter == 0){
            ui->label_conductor->setText("");
            QMessageBox::critical(this,"data","Conductor no registrado");
        }
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
        QHash<QString,QString> current;
        current.insert("salida_base", objetoxd.toObject().value("salida_base").toString());
        current.insert("Inicio_ruta", objetoxd.toObject().value("Inicio_ruta").toString());
        current.insert("Final_ruta", objetoxd.toObject().value("Final_ruta").toString());
        current.insert("Abandono_ruta", objetoxd.toObject().value("Abandono_ruta").toString());
        current.insert("Ingreso_relleno",objetoxd.toObject().value("Ingreso_relleno").toString());
        current.insert("Salida_relleno",objetoxd.toObject().value("Salida_relleno").toString());
        current.insert("Inicio_almuerzo",objetoxd.toObject().value("Inicio_almuerzo").toString());
        current.insert("Final_almuerzo",objetoxd.toObject().value("Final_almuerzo").toString());
        current.insert("Regreso_base",objetoxd.toObject().value("Regreso_base").toString());

        current.insert("ruta",objetoxd.toObject().value("ruta").toString());
        current.insert("ayudantes",objetoxd.toObject().value("ayudantes").toString());
        current.insert("conductor",objetoxd.toObject().value("conductor").toString());
        current.insert("movil",objetoxd.toObject().value("movil").toString());

        //local_movil.insert(objetoxd.toObject().value("movil").toString(),current);
        local_movil.insert(objetoxd.toObject().value("salida_base").toString(),current);
    }
}

void Registro_horarios::read_done(){

    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/done_horarios.txt";
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
        current.insert("salida_base", objetoxd.toObject().value("salida_base").toString());
        current.insert("Inicio_ruta", objetoxd.toObject().value("Inicio_ruta").toString());
        current.insert("Final_ruta", objetoxd.toObject().value("Final_ruta").toString());
        current.insert("Abandono_ruta", objetoxd.toObject().value("Abandono_ruta").toString());
        current.insert("Ingreso_relleno",objetoxd.toObject().value("Ingreso_relleno").toString());
        current.insert("Salida_relleno",objetoxd.toObject().value("Salida_relleno").toString());
        current.insert("Inicio_almuerzo",objetoxd.toObject().value("Inicio_almuerzo").toString());
        current.insert("Final_almuerzo",objetoxd.toObject().value("Final_almuerzo").toString());
        current.insert("Regreso_base",objetoxd.toObject().value("Regreso_base").toString());

        current.insert("ruta",objetoxd.toObject().value("ruta").toString());
        current.insert("ayudantes",objetoxd.toObject().value("ayudantes").toString());
        current.insert("conductor",objetoxd.toObject().value("conductor").toString());
        current.insert("movil",objetoxd.toObject().value("movil").toString());

        //done.insert(objetoxd.toObject().value("movil").toString(),current);
        done.insert(objetoxd.toObject().value("salida_base").toString(),current);
    }
}

void Registro_horarios::update_table(QHash<QString, QHash<QString,QString>>update){

    //Rewrite the local table
    ui -> table_gral -> setRowCount(0);

    QHashIterator<QString, QHash<QString, QString>>iter(update);
    ui->table_gral->setSortingEnabled(false);
    while(iter.hasNext()){

        auto current = iter.next().key();

        //Add a new row
        int  row_control;
        ui->table_gral->insertRow(ui->table_gral->rowCount());
        row_control= ui->table_gral->rowCount()-1;

        //Writing the current row
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(update[current]["movil"]));
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

        if(update[current]["Abandono_ruta"]!=""){
            ui->table_gral->item(row_control,0)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,1)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,2)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,3)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,4)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,5)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,6)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,7)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,8)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,9)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,10)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,11)->setBackground(QColor("#1D8680"));
            ui->table_gral->item(row_control,12)->setBackground(QColor("#1D8680"));
        }
        else{
            ui->table_gral->item(row_control,0)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,1)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,2)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,3)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,4)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,5)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,6)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,7)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,8)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,9)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,10)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,11)->setBackground(QColor("#EBEDED"));
            ui->table_gral->item(row_control,12)->setBackground(QColor("#EBEDED"));
        }
    }
    ui->table_gral->setSortingEnabled(true);
    ui->table_gral->sortByColumn(0,Qt::AscendingOrder);
}

void Registro_horarios::update_schedule(QHash<QString, QString>update){
    //Rewrite the local table
    ui -> table_horarios-> setRowCount(0);
    QHash<QString, QString>action_items;

    QStringList actions = {"salida_base",
                                         "Inicio_ruta",
                                         "Final_ruta",
                                         "Abandono_ruta",
                                         "Ingreso_relleno",
                                         "Salida_relleno",
                                         "Inicio_almuerzo",
                                         "Final_almuerzo",
                                         "Regreso_base"};

    action_items["salida_base"] = "00 - Salida Base";
    action_items["Inicio_ruta"] = "11 - Inicio Ruta";
    action_items["Final_ruta"] = "12 - Final Ruta";
    action_items["Abandono_ruta"] = "13 - Abandono Ruta";
    action_items["Ingreso_relleno"] = "21 - Ingreso Relleno";
    action_items["Salida_relleno"] = "22 - Salida Relleno";
    action_items["Inicio_almuerzo"] = "31 - Inicio Almuerzo";
    action_items["Final_almuerzo"] = "32 - Final Almuerzo";
    action_items["Regreso_base"] = "99 - Regreso Base";

    foreach (QString item, actions) {
        //Add a new row
        int  row_control;
        ui -> table_horarios -> insertRow(ui->table_horarios->rowCount());
        row_control= ui->table_horarios->rowCount()-1;

        //Writing the current row
        ui->table_horarios->setItem(row_control, 0, new QTableWidgetItem(action_items[item]));
        ui->table_horarios->setItem(row_control, 1, new QTableWidgetItem(update[item]));
    }
}

void Registro_horarios::on_boton_registrar_clicked()
{
    QString movil = ui -> label_movil -> text();
    QString ruta = ui -> label_ruta -> text();
    QString conductor = ui -> label_conductor -> text();
    QString ayudantes = ui -> label_ayudantes -> text();
    QString time = QDateTime::currentDateTime().toString("dd.MM.yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    QString random = search_car(movil); // This function search the movil and returns its ID

    ui -> label_search -> setText("");

    if(movil!="" && ruta!="" && conductor!="" && ayudantes!=""){

        //In the condition  local_movil[movil]["salida_base"] ==""
        if (random ==""){

            //+local_movil[time]["movil"] = movil;
            local_movil[time]["movil"] = movil;
            local_movil[time]["ruta"] = ruta;
            local_movil[time]["conductor"] = conductor;
            local_movil[time]["ayudantes"] = ayudantes;
            local_movil[time]["salida_base"] = time  ;

            //Save the pendant data of register
            save("pendant");
            update_table(local_movil);

            //Restart the values from every text edit
            ui -> label_movil -> setText("");
            ui -> label_ruta -> setText("");
            ui -> label_conductor -> setText("");
            ui -> label_ayudantes -> setText("");

            //Set a completer for the search button
            QHashIterator<QString, QHash<QString, QString>>search_iter(local_movil);
            QStringList searching;
            QString random;
            while(search_iter.hasNext()){
                random = local_movil[search_iter.next().key()]["movil"];
                searching << random;
            }

            std::sort(searching.begin(), searching.end());
            QCompleter *search_completer = new QCompleter(searching,this);

            search_completer -> setCaseSensitivity(Qt::CaseInsensitive);
            search_completer -> setCompletionMode(QCompleter::PopupCompletion);
            search_completer -> setFilterMode(Qt::MatchStartsWith);
            ui -> label_search -> setCompleter(search_completer);

        }
        else{
            QMessageBox::critical(this,"data","Este vehículo todavía no concluyó con su ciclo");
        }
    }
    else{
        QMessageBox::critical(this,"data","Rellenar todos los campos porfavor");
    }
}

void Registro_horarios::on_search_item_clicked()
{
    QString search = ui -> label_search -> text();
    QString random = search_car(search);

    int counter = 0;
    //Search for the register in the table
    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);
    while(iter.hasNext()){
        auto current = iter.next().key();
        if (current==random){
            counter = 1;
            break;
        }
    }

    if(counter == 1){
        ui -> frame_movil -> setText(local_movil[random]["movil"]);
        ui -> frame_ruta -> setText(local_movil[random]["ruta"]);
        ui -> frame_conductor -> setText(local_movil[random]["conductor"]);
        ui -> frame_ayudantes -> setText(local_movil[random]["ayudantes"]);
        update_schedule(local_movil[random]);

        //Restart the action selector TODO - Suggest the next action
        ui -> selector-> setEditable(true);
        ui -> selector-> setCurrentIndex(-1);
        ui -> selector-> setCurrentText("Seleccionar item");

        vehicle_exists =true;
    }
    else{
        if(search!=""){
            QMessageBox::critical(this,"data","No se registró una salida de base para este vehículo");
             ui->label_search->setText("");
        }
         vehicle_exists = false;
    }
}

void Registro_horarios::on_button_add_clicked()
{
    QString action =ui -> selector -> currentText();
    QString actual = ui -> label_search -> text();
    QString time = ui -> label_date -> text();
    QString auxiliar;
    QString stat = "no_erase";
    QString random = search_car(actual);
    QStringList abandoned;
    int locker = 0;

    if(vehicle_exists){
        if(action!="Seleccionar item"){
            if(actual!=""){
                if(action == "11 - Inicio Ruta"){
                    auxiliar ="Inicio_ruta";
                }
                else if(action ==  "12 - Final Ruta"){
                    auxiliar ="Final_ruta";
                }
                else if (action == "13 - Abandono Ruta"){
                    auxiliar ="Abandono_ruta";
                }
                else if (action == "21 - Ingreso Relleno Sanitario"){
                    auxiliar ="Ingreso_relleno";
                }
                else if(action ==  "22 - Salida Relleno Sanitario"){
                    auxiliar ="Salida_relleno";
                }
                else if(action == "31 - Inicio Almuerzo"){
                    QTimer::singleShot(5000, [=](){
                        alarm_function(actual);
                    });
                    auxiliar ="Inicio_almuerzo";
                }
                else if (action == "32- Final Almuerzo"){
                    auxiliar ="Final_almuerzo";
                }
                else if(action == "99 - Regreso Base"){
                    //In this case we have to close the cycle
                    if(local_movil[random]["Abandono_ruta"] == ""){
                        stat = "erase";
                    }
                    else{
                        stat = "no_erase";
                    }
                    auxiliar ="Regreso_base";
                }
                else{

                    ui -> selector-> setEditable(true);
                    ui -> selector-> setCurrentIndex(-1);
                    ui -> selector-> setCurrentText("Seleccionar item");

                    locker = 1;
                }
                if(locker==0){
                    if(local_movil[random][auxiliar]==""){

                        if (stat =="no_erase"){
                            local_movil[random][auxiliar] = time;
                            update_schedule(local_movil[random]);
                            update_table(local_movil);
                            save("pendant");
                        }
                        else if(stat == "erase"){
                            local_movil[random][auxiliar] = time;
                            abandoned = eliminate_register(actual);

                            foreach (QString saver, abandoned) {
                                QHash<QString, QString> data;
                                data["salida_base"] = local_movil[saver]["salida_base"];
                                data["Inicio_ruta"] = local_movil[saver]["Inicio_ruta"];
                                data["Final_ruta"] = local_movil[saver]["Final_ruta"];
                                data["Abandono_ruta"] = local_movil[saver]["Abandono_ruta"];
                                data["Ingreso_relleno"] = local_movil[saver]["Ingreso_relleno"];
                                data["Salida_relleno"] = local_movil[saver]["Salida_relleno"];
                                data["Inicio_almuerzo"] = local_movil[saver]["Inicio_almuerzo"];
                                data["Final_almuerzo"] = local_movil[saver]["Final_almuerzo"];
                                data["Regreso_base"] = local_movil[saver]["Regreso_base"];
                                data["ruta"] = local_movil[saver]["ruta"];
                                data["conductor"] = local_movil[saver]["conductor"];
                                data["ayudantes"] = local_movil[saver]["ayudantes"];
                                data["movil"] = local_movil[saver]["movil"];
                                done[saver] = data;
                            }
                            //Add the new data to a temporal variable for saving
                            QHash<QString, QString> data;
                            data["salida_base"] = local_movil[random]["salida_base"];
                            data["Inicio_ruta"] = local_movil[random]["Inicio_ruta"];
                            data["Final_ruta"] = local_movil[random]["Final_ruta"];
                            data["Abandono_ruta"] = local_movil[random]["Abandono_ruta"];
                            data["Ingreso_relleno"] = local_movil[random]["Ingreso_relleno"];
                            data["Salida_relleno"] = local_movil[random]["Salida_relleno"];
                            data["Inicio_almuerzo"] = local_movil[random]["Inicio_almuerzo"];
                            data["Final_almuerzo"] = local_movil[random]["Final_almuerzo"];
                            data["Regreso_base"] = local_movil[random]["Regreso_base"];
                            data["ruta"] = local_movil[random]["ruta"];
                            data["conductor"] = local_movil[random]["conductor"];
                            data["ayudantes"] = local_movil[random]["ayudantes"];
                            data["movil"] = local_movil[random]["movil"];

                            //++Rpelaced actual with random

                            done[random] = data;
                            update_schedule(local_movil[random]);
                            update_table(local_movil);
                            eliminate_data<<abandoned;
                            eliminate_data<<random;
                            save("pendant");
                            save("done");
                        }
                    }
                    else{
                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "Registro existente", "Desea sobreescribir?",QMessageBox::Yes|QMessageBox::No);

                        if(reply == QMessageBox::Yes){
                            //TODO ADD SAVING HERE
                            if (stat =="no_erase"){
                                local_movil[random][auxiliar] = time;
                                update_schedule(local_movil[random]);
                                update_table(local_movil);
                                save("pendant");
                            }
                            else if(stat == "erase"){
                                local_movil[random][auxiliar] = time;
                                abandoned = eliminate_register(actual);

                                foreach (QString saver, abandoned) {
                                    QHash<QString, QString> data;
                                    data["salida_base"] = local_movil[saver]["salida_base"];
                                    data["Inicio_ruta"] = local_movil[saver]["Inicio_ruta"];
                                    data["Final_ruta"] = local_movil[saver]["Final_ruta"];
                                    data["Abandono_ruta"] = local_movil[saver]["Abandono_ruta"];
                                    data["Ingreso_relleno"] = local_movil[saver]["Ingreso_relleno"];
                                    data["Salida_relleno"] = local_movil[saver]["Salida_relleno"];
                                    data["Inicio_almuerzo"] = local_movil[saver]["Inicio_almuerzo"];
                                    data["Final_almuerzo"] = local_movil[saver]["Final_almuerzo"];
                                    data["Regreso_base"] = local_movil[saver]["Regreso_base"];
                                    data["ruta"] = local_movil[saver]["ruta"];
                                    data["conductor"] = local_movil[saver]["conductor"];
                                    data["ayudantes"] = local_movil[saver]["ayudantes"];
                                    data["movil"] = local_movil[saver]["movil"];
                                    done[saver] = data;
                                }

                                //Add the new data to a temporal variable for saving
                                QHash<QString, QString> data;
                                data["salida_base"] = local_movil[random]["salida_base"];
                                data["Inicio_ruta"] = local_movil[random]["Inicio_ruta"];
                                data["Final_ruta"] = local_movil[random]["Final_ruta"];
                                data["Abandono_ruta"] = local_movil[random]["Abandono_ruta"];
                                data["Ingreso_relleno"] = local_movil[random]["Ingreso_relleno"];
                                data["Salida_relleno"] = local_movil[random]["Salida_relleno"];
                                data["Inicio_almuerzo"] = local_movil[random]["Inicio_almuerzo"];
                                data["Final_almuerzo"] = local_movil[random]["Final_almuerzo"];
                                data["Regreso_base"] = local_movil[random]["Regreso_base"];
                                data["ruta"] = local_movil[random]["ruta"];
                                data["conductor"] = local_movil[random]["conductor"];
                                data["ayudantes"] = local_movil[random]["ayudantes"];
                                data["movil"] = local_movil[random]["movil"];

                                eliminate_data<<abandoned;
                                eliminate_data<<random;
                                done[random] = data;
                                update_schedule(local_movil[random]);
                                update_table(local_movil);

                                save("pendant");
                                save("done");
                            }
                        }
                    }
                }
                else{
                    QMessageBox::critical(this,"data","Acción no válida");
                }
            }
            else{
                QMessageBox::critical(this,"data","Seleccionar un vehículo porfavor");
            }
        }
        else{
             QMessageBox::critical(this,"data","Seleccionar una acción a realizar porfavor");
        }
    }
    else{
        QMessageBox::critical(this,"data","Vehículo no registro salida de Base");
    }
}


void Registro_horarios::on_button_erase_clicked()
{
    QString action =ui -> selector -> currentText();
    QString actual = ui -> label_search -> text();
    QString time = ui -> label_date -> text();
    QString auxiliar;
    QString random = search_car(actual);
    int locker = 0;

    if(action!="Seleccionar item"){
        if(actual!=""){
            if(action == "11 - Inicio Ruta"){
                auxiliar ="Inicio_ruta";
            }
            else if(action ==  "12 - Final Ruta"){
                auxiliar ="Final_ruta";
            }
            else if (action == "13 - Abandono Ruta"){
                auxiliar ="Abandono_ruta";
            }
            else if (action == "21 - Ingreso Relleno Sanitario"){
                auxiliar ="Ingreso_relleno";
            }
            else if(action ==  "22 - Salida Relleno Sanitario"){
                auxiliar ="Salida_relleno";
            }
            else if(action == "31 - Inicio Almuerzo"){
                auxiliar ="Inicio_almuerzo";
            }
            else if (action == "32- Final Almuerzo"){
                auxiliar ="Final_almuerzo";
            }
            else if(action == "99 - Regreso Base"){
                //In this case we have to close the cycle
                auxiliar ="Regreso_base";
            }
            else{
                locker = 1;
            }

            if(locker==0){
                if(local_movil[random][auxiliar]!=""){
                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(this, "Borrar  "+ auxiliar, "Seguro quiere borrar esa hora?",QMessageBox::Yes|QMessageBox::No);

                    if(reply == QMessageBox::Yes){
                        local_movil[random][auxiliar]="";
                        update_schedule(local_movil[random]);
                        update_table(local_movil);
                        save("pendant");

                    }
                }
                else{
                     QMessageBox::critical(this,"data","Valor no registrado");
                }
            }
            else{
                QMessageBox::critical(this,"data","Acción no válida");
            }
        }
        else{
            QMessageBox::critical(this,"data","Seleccionar un vehículo porfavor");
        }
    }
    else{
         QMessageBox::critical(this,"data","Seleccionar una acción a realizar porfavor");
    }
}

void Registro_horarios::save(QString action){

    QJsonDocument documentoxd;
    QJsonObject datosxd;
    QJsonArray arrayDeDatos;
    QHash<QString, QHash<QString, QString>>saver;
    if(action == "pendant"){
        saver = local_movil;
    }
    else{
        saver = done;
    }

    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();
        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;
        //currnt.insert("movil",item);
        while(it_2.hasNext()){
            auto valores=it_2.next();
            currnt.insert(valores.key(),valores.value());
        }
        arrayDeDatos.append(currnt);
     }

    documentoxd.setArray(arrayDeDatos);
    QString path = QDir::homePath();

    QDir any;
    any.mkdir(path+"/LPL_documents");

    QString filename= path+"/LPL_documents/"+action+"_horarios.txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(documentoxd.toJson());
    file.close();
}

void Registro_horarios::on_close_button_clicked()
{
    foreach (QString item, eliminate_data) {
        local_movil.remove(item);
        save("pendant");
    }
    emit logOut();
}

QString Registro_horarios::search_car(QString item){

    //Start a QIterator for the local movil where we are searching the movil
    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);
    QString key_search;

    while(iter.hasNext()){

        auto current = iter.next().key();
        if(local_movil[current]["movil"]==item &&  local_movil[current]["Regreso_base"]==""){
            key_search = current;
            break;
        }
    }
    return key_search;
}

QStringList Registro_horarios::eliminate_register(QString movil){
    QStringList eliminates;
    QStringList keys;
    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);

    while(iter.hasNext()){

        auto current = iter.next().key();
        if(local_movil[current]["movil"]==movil &&  local_movil[current]["Abandono_ruta"]!=""){
            keys<<current;
        }

    }
    return keys;
}

void Registro_horarios::alarm_function(QString movil){

    QMessageBox::warning(this,"Almuerzo Terminado","Movil: "+movil);
}
