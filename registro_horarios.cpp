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
#include <QSound>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

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
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.3));
    ui -> frame_9 -> setFixedHeight(static_cast<int>(height*0.333));
    ui -> frame_12 -> setFixedHeight(static_cast<int>(height*0.05));

    //adjust the width
     ui -> frame_6 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_5 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_7 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_8 -> setFixedWidth(static_cast<int>(width*0.1));

     ui -> frame_13 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_14 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_15 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_16 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_17 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_18 -> setFixedWidth(static_cast<int>(width*0.15));

     //Setting all the icons
     double pix_w_b = (width*100)/1920;
     double pix_h_b = (height*100)/1080;

     double pix_w_c = (width*80)/1920;
     double pix_h_c = (height*80)/1080;

     QPixmap pix_base(":/images/img/base.png");
     ui->icon_base->setPixmap(pix_base.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_base->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

     QPixmap pix_ruta(":/images/img/ruta-verde.png");
     ui->icon_ruta_2->setPixmap(pix_ruta.scaled( static_cast<int>(pix_w_c),static_cast<int>(pix_h_c), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_ruta_2->setFixedSize(static_cast<int>(pix_w_c), static_cast<int>(pix_h_c));

     QPixmap pix_relleno(":/images/img/relleno.png");
     ui->icon_relleno->setPixmap(pix_relleno.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_relleno->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

     QPixmap pix_almuerzo(":/images/img/almuerzo-verde.png");
     ui->icon_almuerzo->setPixmap(pix_almuerzo.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_almuerzo->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));


    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    //Set the table Size
    ui -> table_gral -> setColumnCount(15);
    for(int r=0; r<13; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/14.71));
    }
    ui->table_gral ->setColumnWidth(14,0);

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
                           "Regreso a base",
                           "Comentarios"};

    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    read_temporal();
    //read_done();

    auxiliar_value = "";
    update_table(local_movil);

    //Set icons
    double icon_w = (width*50)/1920;
    double icon_h = (height*50)/1080;

    QPixmap icon_movil(":/images/img/camion-blanco.png");
    ui->icon_movil->setPixmap(icon_movil.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_movil->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_ruta(":/images/img/ruta-blanco.png");
    ui->icon_ruta->setPixmap(icon_ruta.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ruta->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_conductor(":/images/img/conductor-blanco.png");
    ui->icon_conductor->setPixmap(icon_conductor.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_conductor->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    QPixmap icon_ayudantes(":/images/img/ayudantes-blanco.png");
    ui->icon_ayudantes->setPixmap(icon_ayudantes.scaled( static_cast<int>(icon_w),static_cast<int>(icon_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ayudantes->setFixedSize(static_cast<int>(icon_w), static_cast<int>(icon_h));

    //Set Search icons
    QPixmap pix_b1(":/images/img/search_2.png");
    QPixmap pix_b2(":/images/img/flecha-blanca.png");
    QPixmap pix_b3(":/images/img/equis-blanca.png");

    QIcon ButtonIcon(pix_b1);
    QIcon ButtonIcon_2(pix_b2);
    QIcon ButtonIcon_3(pix_b3);

    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));

    ui->search_rbase->setIcon(ButtonIcon_2);
    ui->search_rbase->setIconSize(QSize(15,15));

    ui->search_iruta->setIcon(ButtonIcon_2);
    ui->search_iruta->setIconSize(QSize(15,15));

    ui->search_artua->setIcon(ButtonIcon_2);
    ui->search_artua->setIconSize(QSize(15,15));

    ui->search_fruta->setIcon(ButtonIcon_2);
    ui->search_fruta->setIconSize(QSize(15,15));

    ui->search_irelleno->setIcon(ButtonIcon_2);
    ui->search_irelleno->setIconSize(QSize(15,15));

    ui->search_srelleno->setIcon(ButtonIcon_2);
    ui->search_srelleno->setIconSize(QSize(15,15));

    ui->search_ialmuerzo->setIcon(ButtonIcon_2);
    ui->search_ialmuerzo->setIconSize(QSize(15,15));

    ui->search_falmuerzo->setIcon(ButtonIcon_2);
    ui->search_falmuerzo->setIconSize(QSize(15,15));


    //Erase button Icons

    ui -> erase_rbase -> setIcon(ButtonIcon_3);
    ui -> erase_rbase -> setIconSize(QSize(15,15));

    ui -> erase_iruta -> setIcon(ButtonIcon_3);
    ui -> erase_iruta -> setIconSize(QSize(15,15));

    ui -> erase_aruta -> setIcon(ButtonIcon_3);
    ui -> erase_aruta -> setIconSize(QSize(15,15));

    ui -> erase_fruta -> setIcon(ButtonIcon_3);
    ui -> erase_fruta -> setIconSize(QSize(15,15));

    ui -> erase_irelleno -> setIcon(ButtonIcon_3);
    ui -> erase_irelleno -> setIconSize(QSize(15,15));

    ui -> erase_srelleno -> setIcon(ButtonIcon_3);
    ui -> erase_srelleno -> setIconSize(QSize(15,15));

    ui -> erase_ialmuerzo -> setIcon(ButtonIcon_3);
    ui -> erase_ialmuerzo -> setIconSize(QSize(15,15));

    ui -> erase_falmuerzo -> setIcon(ButtonIcon_3);
    ui -> erase_falmuerzo -> setIconSize(QSize(15,15));


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
    connect(this, SIGNAL(pressSearchButton()),ui->search_item, SLOT(click()));

    //////////////////////////////////////////////////////////////////////
    ////////////////////////////COMPLETERS////////////////////////////
    /////////////////////////////////////////////////////////////////////

    //Set a completer for the search button
    QHashIterator<QString, QHash<QString, QString>>search_iter(local_movil);
    QStringList searching;
    QString random;
    while(search_iter.hasNext()){
        random = local_movil[search_iter.next().key()]["movil"];
        searching << random;
        qDebug()<<random;
    }

    searching.removeDuplicates();

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

    //Eliminate all values in the searching completer
    searching_completer = searching;

    ui -> button_update -> setDisabled(true);
    current_id="";
    current_car="";

    global_session = "yes";

}

    Registro_horarios::~Registro_horarios()
{
    delete ui;
}

void Registro_horarios::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->label_date->setText(tiempo);
}

void Registro_horarios::get_url(QString receiver){
    this -> url = receiver;
}

void Registro_horarios::get_data(QString real_name, QString user_name, QString token){
    ui->label_user->setText(real_name);
    this -> token = token;
    this -> user_name = user_name;
    from_db_readStaff();
    from_db_readVehicles();
    from_db_readLink_2();
    from_db_readLink_1();
    from_db_readRoutes();
}

void Registro_horarios::set_data(){

    QString actual_item = ui->label_movil->text();
    if(actual_item!=""){
        if(db_vehiculos[actual_item]["movil"]!=""){

            //TODO Add a condition here for changing the route in case of the time
            ui -> label_ruta -> setText(db_rutas[db_link_RV[actual_item]["ruta"]]["ruta"]);

            //TODO switch betwen the driver one and two
            ui -> label_conductor -> setText(db_personal[db_link_VP[actual_item]["personal"]]["nombre"]);
            ui -> label_ayudantes -> setText(db_vehiculos[actual_item]["numeroDeAyudantes"]);
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

        QHashIterator<QString,  QHash<QString, QString>> iter(db_rutas);
        int counter = 0;
        while(iter.hasNext()){
            auto the_key = iter.next().key();
            if(db_rutas[the_key]["ruta"]==actual_item){
                counter = 1;
                break;
            }
        }
        if(counter != 1){
            ui->label_ruta->setText("");
            QMessageBox::critical(this,"data","Ruta Inválida");
        }
    }
}

void Registro_horarios::set_conductor(){

    QString actual_item = ui->label_conductor->text();
    int counter = 0;
    if(actual_item!=""){

        QHashIterator<QString, QHash<QString, QString>>iter(db_personal);

        while(iter.hasNext()){
            auto current = iter.next().key();
            if(db_personal[current]["nombre"]==actual_item){
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

        current.insert("Inicio_ruta_b", objetoxd.toObject().value("Inicio_ruta_b").toString());
        current.insert("Final_ruta_b", objetoxd.toObject().value("Final_ruta_b").toString());
        current.insert("Abandono_ruta_b", objetoxd.toObject().value("Abandono_ruta_b").toString());
        current.insert("Ingreso_relleno_b",objetoxd.toObject().value("Ingreso_relleno_b").toString());
        current.insert("Salida_relleno_b",objetoxd.toObject().value("Salida_relleno_b").toString());
        current.insert("Inicio_almuerzo_b",objetoxd.toObject().value("Inicio_almuerzo_b").toString());
        current.insert("Final_almuerzo_b",objetoxd.toObject().value("Final_almuerzo_b").toString());
        current.insert("Regreso_base_b",objetoxd.toObject().value("Regreso_base_b").toString());

        current.insert("ruta",objetoxd.toObject().value("ruta").toString());
        current.insert("ruta_id",objetoxd.toObject().value("ruta_id").toString());
        current.insert("ayudantes",objetoxd.toObject().value("ayudantes").toString());
        current.insert("conductor",objetoxd.toObject().value("conductor").toString());
        current.insert("conductor_id",objetoxd.toObject().value("conductor_id").toString());
        current.insert("movil",objetoxd.toObject().value("movil").toString());

        current.insert("id",objetoxd.toObject().value("id").toString());
        current.insert("virtual_id",objetoxd.toObject().value("virtual_id").toString());

        current.insert("modification",objetoxd.toObject().value("modification").toString());

        current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());

        //local_movil.insert(objetoxd.toObject().value("movil").toString(),current);
        local_movil.insert(objetoxd.toObject().value("id").toString(),current);
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

        current.insert("Inicio_ruta_b", objetoxd.toObject().value("Inicio_ruta_b").toString());
        current.insert("Final_ruta_b", objetoxd.toObject().value("Final_ruta_b").toString());
        current.insert("Abandono_ruta_b", objetoxd.toObject().value("Abandono_ruta_b").toString());
        current.insert("Ingreso_relleno_b",objetoxd.toObject().value("Ingreso_relleno_b").toString());
        current.insert("Salida_relleno_b",objetoxd.toObject().value("Salida_relleno_b").toString());
        current.insert("Inicio_almuerzo_b",objetoxd.toObject().value("Inicio_almuerzo_b").toString());
        current.insert("Final_almuerzo_b",objetoxd.toObject().value("Final_almuerzo_b").toString());
        current.insert("Regreso_base_b",objetoxd.toObject().value("Regreso_base_b").toString());

        current.insert("ruta",objetoxd.toObject().value("ruta").toString());
        current.insert("ayudantes",objetoxd.toObject().value("ayudantes").toString());
        current.insert("conductor",objetoxd.toObject().value("conductor").toString());
        current.insert("conductor_id",objetoxd.toObject().value("conductor_id").toString());
        current.insert("movil",objetoxd.toObject().value("movil").toString());

        current.insert("modification",objetoxd.toObject().value("modification").toString());
        current.insert("id",objetoxd.toObject().value("id").toString());
        current.insert("virtual_id",objetoxd.toObject().value("virtual_id").toString());

        current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());

        //done.insert(objetoxd.toObject().value("movil").toString(),current);
        done.insert(objetoxd.toObject().value("id").toString(),current);
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
        ui->table_gral->setItem(row_control, 13, new QTableWidgetItem(update[current]["comentarios"]));
        ui->table_gral->setItem(row_control, 14, new QTableWidgetItem(update[current]["id"]));

        if(update[current]["Abandono_ruta"]!=""){
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
            ui->table_gral->item(row_control,13)->setBackground(QColor("#EBEDED"));
        }
        else if(update[current]["Regreso_base"]!=""&&update[current]["Abandono_ruta"]==""){
            ui->table_gral->item(row_control,0)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,1)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,2)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,3)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,4)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,5)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,6)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,7)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,8)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,9)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,10)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,11)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,12)->setBackground(QColor("#B7E1DF"));
            ui->table_gral->item(row_control,13)->setBackground(QColor("#B7E1DF"));
        }
        else{
            ui->table_gral->item(row_control,0)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,1)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,2)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,3)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,4)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,5)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,6)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,7)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,8)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,9)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,10)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,11)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,12)->setBackground(QColor("#FFFFFF"));
            ui->table_gral->item(row_control,13)->setBackground(QColor("#FFFFFF"));
        }
    }
    ui->table_gral->setSortingEnabled(true);
    ui->table_gral->sortByColumn(0,Qt::AscendingOrder);
}

void Registro_horarios::on_boton_registrar_clicked()
{
    QString movil = ui -> label_movil -> text();
    QString ruta = ui -> label_ruta -> text();
    QString conductor = ui -> label_conductor -> text();
    QString ayudantes = ui -> label_ayudantes -> text();
    QString time = ui->label_date->text();
    QString random = search_car(movil); // This function search the movil and returns its ID
    QString comentarios = ui->text_comentarios->toPlainText();
    QString second_id;

    qlonglong stamp = QDateTime::fromString(time, "dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch();

    ui -> label_search -> setText("");

    if(movil!="" && ruta!="" && conductor!="" && ayudantes!=""){

        //In the condition  local_movil[movil]["salida_base"] ==""
        if (random ==""){

            //+local_movil[time]["movil"] = movil;
            local_movil[time]["movil"] = movil;

            QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);
            QHashIterator<QString, QHash<QString, QString>> iter_routes(db_rutas);

            while(iter_staff.hasNext()){
                    auto staff_id = iter_staff.next().key();

                    if(db_personal[staff_id]["nombre"] == conductor){
                           local_movil[time]["conductor_id"] = staff_id;
                           break;
                    }
            }

            while(iter_routes.hasNext()){
                    auto routes_id = iter_routes.next().key();

                    if(db_rutas[routes_id]["ruta"] == ruta){
                           local_movil[time]["ruta_id"] = routes_id;
                           break;
                    }
            }

            local_movil[time]["ruta"] = ruta;
            local_movil[time]["conductor"] = conductor;
            local_movil[time]["ayudantes"] = ayudantes;
            local_movil[time]["salida_base"] = time  ;
            local_movil[time]["comentarios"] = comentarios;
            local_movil[time]["id"] = time;
            local_movil[time]["salida_base_sender"] = QString::number(stamp);


            //Restart the values from every text edit
            ui -> label_movil -> setText("");
            ui -> label_ruta -> setText("");
            ui -> label_conductor -> setText("");
            ui -> label_ayudantes -> setText("");
            ui -> text_comentarios -> setPlainText("");

            //We need to search if this vehicle has a relation between other cars
            second_id = search_relation(movil, time);

            //local_movil[time]["concluded"];
            local_movil[time]["virtual_id"] = second_id; //ADD VIRTUAL ID

            //Save the pendant data of register
            save("pendant");
            auxiliar_value = "";
            update_table(local_movil);

            //Set a completer for the search button
            searching_completer<<movil;

            searching_completer.removeDuplicates();
            std::sort(searching_completer.begin(), searching_completer.end());

            QCompleter *search_completer = new QCompleter(searching_completer,this);

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

//This search function is going to work as a filter
void Registro_horarios::on_search_item_clicked()
{
    QString search = ui -> label_search -> text();

    QHash<QString, QHash<QString, QString>> auxiliar_search;

    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);

    int counter = 0;

    if(search!=""){
        while(iter.hasNext()){
            auto current = iter.next().key();

            if(local_movil[current]["movil"]==search){
                 auxiliar_search[current] = local_movil[current];
                 counter++;
            }
        }

        if(counter!=0){
            auxiliar_value = "";
            update_table(auxiliar_search);
            qDebug()<<counter;
        }
        else{
            QMessageBox::critical(this,"data","No se registró una salida de base para este vehículo");
            ui->label_search->setText("");
        }
    }

    ui ->label_search -> setText("");
}

//Clean the filters applied to the main table
void Registro_horarios::on_pushButton_clicked()
{
    auxiliar_value = "";
    update_table(local_movil);
}

//This will play as an esc Function to cancel a register
void Registro_horarios::on_butto_cancel_clicked()
{
    //create empty hash
    QHash<QString,QString>empty;

    //clear all text fields
    ui -> frame_movil -> setText("");
    ui -> frame_ruta -> setText("");
    ui -> frame_conductor -> setText("");
    ui -> frame_ayudantes -> setText("");

    ui -> label_movil -> setText("");
    ui -> label_ruta -> setText("");
    ui -> label_conductor -> setText("");
    ui -> label_ayudantes -> setText("");
    ui -> text_comentarios -> setPlainText("");

    ui -> salida_base -> setText("");
    ui -> regreso_base -> setText("");
    ui -> inicio_ruta -> setText("");
    ui -> abandono_ruta -> setText("");
    ui -> final_ruta -> setText("");
    ui -> ingreso_relleno -> setText("");
    ui -> salida_relleno -> setText("");
    ui -> inicio_almuerzo -> setText("");
    ui -> final_almuerzo -> setText("");

    current_id = "";
    current_car = "";
    auxiliar_value = "";

    ui -> boton_registrar -> setEnabled(true);
    ui -> button_update -> setDisabled(true);
}

//This allows you to update your data in case something was wrong
void Registro_horarios::on_button_update_clicked()
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Actualización de registro", "Seguro desea actualizar este registro?",QMessageBox::Yes|QMessageBox::No);

    if(reply == QMessageBox::Yes){

        QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);
        QHashIterator<QString, QHash<QString, QString>> iter_routes(db_rutas);

        while(iter_staff.hasNext()){
                auto staff_id = iter_staff.next().key();

                if(db_personal[staff_id]["nombre"] ==  ui -> label_conductor -> text()){
                       local_movil[current_id]["conductor_id"] = staff_id;
                       break;
                }
        }

        while(iter_routes.hasNext()){
                auto routes_id = iter_routes.next().key();

                if(db_rutas[routes_id]["ruta"] == ui -> label_ruta -> text()){
                       local_movil[current_id]["ruta_id"] = routes_id;
                       break;
                }
        }

        local_movil[current_id]["movil"] = ui -> label_movil -> text();
        local_movil[current_id]["ruta"]= ui -> label_ruta -> text();
        local_movil[current_id]["conductor"] = ui -> label_conductor -> text();
        local_movil[current_id]["ayudantes"] = ui -> label_ayudantes -> text();
        local_movil[current_id]["comentarios"] = ui -> text_comentarios -> toPlainText();

        //SAVE TEMPORAL DATA JIC
        save("pendant");

        auxiliar_value = "";
        update_table(local_movil);
        on_butto_cancel_clicked();

    }
}


//Close button event, here is where we have to put the new logic to avoid overwriting
void Registro_horarios::on_close_button_clicked()
{
    emit close_all();

    QHash<QString, QHash<QString, QString>> db;

    eliminate_data.removeDuplicates();

    foreach (QString item, eliminate_data) {
        if(item!=""){
            done[item] = local_movil[item];
            db[item] = local_movil[item];
            local_movil.remove(item);
        }
    }

    //save("done");
    save("pendant");

    saveJson(db); // This function should send the array to the database
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

//This function helps to know which registers have an specific action and eliminates it after the cycle finishes
//TODO take a look after the logic change

QStringList Registro_horarios::eliminate_register(QString id_v){
    QStringList eliminates;
    QStringList keys;
    QHashIterator<QString, QHash<QString, QString>>iter(local_movil);

    while(iter.hasNext()){

        auto current = iter.next().key();
//        if(local_movil[current]["movil"]==movil &&  local_movil[current]["Abandono_ruta"]!=""){
//            keys<<current;
//        }
        if(current!=""){
            if(local_movil[current]["virtual_id"] == id_v ){
                keys<<current;
            }
        }
    }
    return keys;
}

//This function activates an alarm that is shown after the user  add an specific key
void Registro_horarios::alarm_function(QString movil,QString id){
    if(local_movil[id]["Final_almuerzo"]==""){
        QSound bells(":/resources/Recursos/alarm.wav");
        bells.play();
        QString time  = QDateTime::currentDateTime().toString("hh:mm");
        QMessageBox::warning(this,"Almuerzo Terminado","Movil: "+movil+ "\n Almuerzo terminado a las "+time+" horas");
        if (QMessageBox::Ok){
            bells.stop();
        }
    }
}


//This function needs to be modified
void Registro_horarios::on_search_rbase_clicked()
{
       //_b register is only visual
       // non _b register should go straight to the data_base
       QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
       QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");

       //the current Id is going to be different from "" only when you press double click in the table
       if (current_id!=""){

           //We verify if the register is empty
           if(local_movil[current_id]["Regreso_base"]==""){

               //We verify if the item before clicking this is route abandonment
               if (local_movil[current_id]["Abandono_ruta"]!=""){

                   //In this case the register should remain in the temporal data after closing session
                   local_movil[current_id]["Regreso_base"] = time;
                   local_movil[current_id]["Regreso_base_b"] = time_b;

                   //The vehicle havent finished its cycle
                   local_movil[current_id]["concluded"] = "pendant";

                   save("pendant");
                   //CHECK THIS PART
               }
               else{

                    //otherwise it should be erased from temporal and be sent to the database
                   local_movil[current_id]["Regreso_base"]=time;
                   local_movil[current_id]["Regreso_base_b"] = time_b;

                   //The vehicle has finished the cycle, we need to search ifit has previous registers to erase
                   search_dependancy(local_movil[current_id]["movil"]);

                   //REVIEW THIS PART
                   //local_movil [current_id]["concluded"] = "concluded";

                   eliminate_data<<current_id;

                   //Also we verify if this register hava abandons before that need to be elminated

                   eliminate_data << eliminate_register(local_movil[current_id]["virtual_id"]);

                    qDebug()<<eliminate_data;
                   //Eliminate this later AND THIS PART SEND STRAIGHT TO THE DATABASE
                   save("pendant");

               }

               //update table and fields
               auxiliar_value = "";
               update_table(local_movil);
               update_fields();

           }
           //Otherwise we ask if they want to overwrite the existing value
           else{
               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
               if(reply == QMessageBox::Yes){

                   //We verify if the item before clicking this is route abandonment
                   if (local_movil[current_id]["Abandono_ruta"]!=""){

                       //In this case the register should remain in the temporal data after closing session
                       local_movil[current_id]["Regreso_base"] = time;
                       local_movil[current_id]["Regreso_base_b"] = time_b;

                       save("pendant");
                       //CHECK THIS PART
                   }
                   else{

                        //otherwise it should be erased from temporal and be sent to the database
                       local_movil[current_id]["Regreso_base"]=time;
                       local_movil[current_id]["Regreso_base_b"] = time_b;

                       eliminate_data<<current_id;

                       //Also we verify if this register hava abandons before that need to be elminated
                       eliminate_data << eliminate_register(local_movil[current_id]["virtual_id"]);
                   }

                   //update table and fields
                   auxiliar_value = "";
                   update_table(local_movil);
                   update_fields();
               }
           }
       }
       else{
           QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
       }
}

void Registro_horarios::on_search_iruta_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Inicio_ruta"]==""){
            local_movil[current_id]["Inicio_ruta"] = time;
            local_movil[current_id]["Inicio_ruta_b"] = time_b;

            auxiliar_value = "";
            update_table(local_movil);
            save("pendant");
            update_fields();
        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Inicio_ruta"] = time;
                local_movil[current_id]["Inicio_ruta_b"] = time_b;

                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
                save("pendant");
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}



void Registro_horarios::on_search_artua_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Abandono_ruta"]==""){
            local_movil[current_id]["Abandono_ruta"] = time;
            local_movil[current_id]["Abandono_ruta_b"] = time_b;

            auxiliar_value = "";
            update_table(local_movil);
            update_fields();
            save("pendant");
        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Abandono_ruta"] = time;
                local_movil[current_id]["Abandono_ruta_b"] = time_b;

                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
                save("pendant");
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_search_fruta_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Final_ruta"]==""){
            local_movil[current_id]["Final_ruta"] = time;
            local_movil[current_id]["Final_ruta_b"] = time_b;

            auxiliar_value = "";
            update_table(local_movil);
            update_fields();
            save("pendant");
        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Final_ruta"] = time;
                local_movil[current_id]["Final_ruta_b"] = time_b;
                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
                save("pendant");
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}


void Registro_horarios::on_search_irelleno_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){

        //We have to verify if the register is empty first
        if(local_movil[current_id]["Ingreso_relleno"]==""){

            //We need to verify if there is a route abandonment first
            if(local_movil[current_id]["Abandono_ruta"]!=""){

                //if there is a route abanonment we need to finish this cycle
                local_movil[current_id]["Ingreso_relleno"] = time;
                local_movil[current_id]["Ingreso_relleno_b"] = time_b;

                //This register is equivalent to Base returnment
                local_movil[current_id]["Regreso_base"] = time;

                //We need to add a new register to this one
                local_movil[current_id]["concluded"] = "pendant";

                save("pendant");
            }
            else{

                //Otherwise is normal register
                local_movil[current_id]["Ingreso_relleno"] = time;
                local_movil[current_id]["Ingreso_relleno_b"] = time_b;
                save("pendant");
            }

            //Reset all fields and tables
            auxiliar_value = "";
            update_table(local_movil);
            update_fields();

        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                //We need to verify if there is a route abandonment first
                if(local_movil[current_id]["Abandono_ruta"]!=""){

                    //if there is a route abanonment we need to finish this cycle
                    local_movil[current_id]["Ingreso_relleno"] = time;
                    local_movil[current_id]["Ingreso_relleno_b"] = time_b;

                    //This register is equivalent to Base returnment
                    local_movil[current_id]["Regreso_base"] = time;

                    save("pendant");
                }
                else{

                    //Otherwise is normal register
                    local_movil[current_id]["Ingreso_relleno"] = time;
                    local_movil[current_id]["Ingreso_relleno_b"] = time_b;
                    save("pendant");
                }

                //Reset all fields and tables
                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_search_srelleno_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Salida_relleno"]==""){

            //We need to verify if there is route abandonment so it will count as a new register
            if(local_movil[current_id]["Abandono_ruta"]!=""){

                QString random = search_car(local_movil[current_id]["movil"]);
                if(random==""){
                    //This shoudld be a new register
                    local_movil[current_id]["Salida_relleno"] = time;
                    local_movil[current_id]["Salida_relleno_b"] = time_b;

                    //This is equivalent to base output

                    QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);
                    QHashIterator<QString, QHash<QString, QString>> iter_routes(db_rutas);

                    while(iter_staff.hasNext()){
                            auto staff_id = iter_staff.next().key();

                            if(db_personal[staff_id]["nombre"] ==  local_movil[current_id]["conductor"]){
                                   local_movil[time]["conductor_id"] = staff_id;
                                   break;
                            }
                    }

                    while(iter_routes.hasNext()){
                            auto routes_id = iter_routes.next().key();

                            if(db_rutas[routes_id]["ruta"] == local_movil[current_id]["ruta"]){
                                   local_movil[time]["ruta_id"] = routes_id;
                                   break;
                            }
                    }

                    local_movil[time]["salida_base"] = time;
                    local_movil[time]["id"] = time;
                    local_movil[time]["movil"] = local_movil[current_id]["movil"];
                    local_movil[time]["conductor"] = local_movil[current_id]["conductor"];
                    local_movil[time]["ayudantes"] = local_movil[current_id]["ayudantes"];
                    local_movil[time]["ruta"] = local_movil[current_id]["ruta"];
                    local_movil[time]["virtual_id"] = local_movil[current_id]["virtual_id"];

                    save("pendant");
                }
                else{
                    QMessageBox::critical(this,"data","Este vehículo no concluyó su ciclo");
                }
            }
            else{
                //This shloud be a normal register
                local_movil[current_id]["Salida_relleno"] = time;
                local_movil[current_id]["Salida_relleno_b"] = time_b;
                save("pendant");
            }

            auxiliar_value = "";
            update_table(local_movil);
            update_fields();

        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Salida_relleno"] = time;
                local_movil[current_id]["Salida_relleno_b"] = time_b;

                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
                save("pendant");
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_search_ialmuerzo_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Inicio_almuerzo"]==""){
            recall(current_car,current_id);
            local_movil[current_id]["Inicio_almuerzo"] = time;
            local_movil[current_id]["Inicio_almuerzo_b"] = time_b;

            auxiliar_value = "";
            update_table(local_movil);
            update_fields();
            save("pendant");
        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Inicio_almuerzo"] = time;
                local_movil[current_id]["Inicio_almuerzo_b"] = time_b;

                auxiliar_value = "";
                update_table(local_movil);
                update_fields();
                save("pendant");
                recall(current_car,current_id);
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_search_falmuerzo_clicked()
{
    QString time_b  = QDateTime::currentDateTime().toString("hh:mm");
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    if (current_id!=""){
        //We can put updates here
        if(local_movil[current_id]["Final_almuerzo"]==""){
            local_movil[current_id]["Final_almuerzo"] = time;
            local_movil[current_id]["Final_almuerzo_b"] = time_b;

            auxiliar_value = "";
            update_table(local_movil);
            update_fields();
            save("pendant");
        }
        else{
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Valor registrado", "Desea sobreescribir este registro?",QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes){
                local_movil[current_id]["Final_almuerzo"] = time;
                local_movil[current_id]["Final_almuerzo_b"] = time_b;

                auxiliar_value = "";
                update_table(local_movil);
                save("pendant");
                update_fields();
            }
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::update_fields(){
    ui -> frame_movil -> setText(local_movil[current_id]["movil"]);
    ui -> frame_ruta -> setText(local_movil[current_id]["ruta"]);
    ui -> frame_conductor -> setText(local_movil[current_id]["conductor"]);
    ui -> frame_ayudantes -> setText(local_movil[current_id]["ayudantes"]);

    ui -> label_movil -> setText(local_movil[current_id]["movil"]);
    ui -> label_ruta -> setText(local_movil[current_id]["ruta"]);
    ui -> label_conductor -> setText(local_movil[current_id]["conductor"]);
    ui -> label_ayudantes -> setText(local_movil[current_id]["ayudantes"]);
    ui -> text_comentarios -> setPlainText(local_movil[current_id]["comentarios"]);

    ui -> salida_base -> setText(local_movil[current_id]["salida_base"]);
    ui -> regreso_base -> setText(local_movil[current_id]["Regreso_base_b"]);
    ui -> inicio_ruta -> setText(local_movil[current_id]["Inicio_ruta_b"]);
    ui -> abandono_ruta -> setText(local_movil[current_id]["Abandono_ruta_b"]);
    ui -> final_ruta -> setText(local_movil[current_id]["Final_ruta_b"]);
    ui -> ingreso_relleno -> setText(local_movil[current_id]["Ingreso_relleno_b"]);
    ui -> salida_relleno -> setText(local_movil[current_id]["Salida_relleno_b"]);
    ui -> inicio_almuerzo -> setText(local_movil[current_id]["Inicio_almuerzo_b"]);
    ui -> final_almuerzo -> setText(local_movil[current_id]["Final_almuerzo_b"]);
}

void Registro_horarios::on_erase_rbase_clicked()
{
       if(current_id!=""){
           QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
           if(reply == QMessageBox::Yes){
               local_movil[current_id]["Regreso_base"]="";
               local_movil[current_id]["Regreso_base_b"]="";
               save("pendant");

               auxiliar_value = "";
               update_table(local_movil);
               update_fields();
               eliminate_data.removeOne(current_id);
           }
       }
       else{
           QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
       }
}

void Registro_horarios::on_erase_iruta_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Inicio_ruta"]="";
            local_movil[current_id]["Inicio_ruta_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_aruta_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Abandono_ruta"]="";
            local_movil[current_id]["Abandono_ruta_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_fruta_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Final_ruta"]="";
            local_movil[current_id]["Final_ruta_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_irelleno_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Ingreso_relleno"]="";
            local_movil[current_id]["Ingreso_relleno_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_srelleno_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Salida_relleno"]="";
            local_movil[current_id]["Salida_relleno_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_ialmuerzo_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Inicio_almuerzo"]="";
            local_movil[current_id]["Inicio_almuerzo_b"]="";
            save("pendant");

            auxiliar_value = "";
            update_fields();
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_erase_falmuerzo_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este valor del registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_movil[current_id]["Final_almuerzo"]="";
            local_movil[current_id]["Final_almuerzo_b"]="";
            save("pendant");

            update_fields();
            auxiliar_value = "";
            update_table(local_movil);
        }
    }
    else{
        QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
    }
}

void Registro_horarios::on_pushButton_2_clicked()
{
    QString time = ui -> label_date -> text();
    QString movil = ui -> label_movil -> text();
    QString random = search_car(movil);
    QString new_id;
     if(current_id!=""){
         if(random==""){
             if(local_movil[current_id]["Regreso_base"]!=""){
                 local_movil[time]["salida_base"] = time;
                 local_movil[time]["id"] = time;
                 local_movil[time]["movil"] = movil;


                 QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);
                 QHashIterator<QString, QHash<QString, QString>> iter_routes(db_rutas);

                 while(iter_staff.hasNext()){
                         auto staff_id = iter_staff.next().key();

                         if(db_personal[staff_id]["nombre"] == ui -> label_conductor -> text()){
                                local_movil[time]["conductor_id"] = staff_id;
                                break;
                         }
                 }

                 while(iter_routes.hasNext()){
                         auto routes_id = iter_routes.next().key();

                         if(db_rutas[routes_id]["ruta"] ==  ui -> label_ruta -> text()){
                                local_movil[time]["ruta_id"] = routes_id;
                                break;
                         }
                 }

                 local_movil[time]["ruta"] = ui -> label_ruta -> text();
                 local_movil[time]["conductor"] = ui -> label_conductor -> text();
                 local_movil[time]["ayudantes"] = ui -> label_ayudantes -> text();
                 new_id = search_relation(local_movil[current_id]["movil"], time);
                 local_movil[time]["virtual_id"] = new_id;

                 auxiliar_value = "";
                 update_table(local_movil);
                 save("pendant");
             }
             else{
                 QMessageBox::critical(this,"data","Este vehículo no concluyó su ciclo aún");
             }
         }
         else{
             QMessageBox::critical(this,"data","Este vehículo no concluyó su ciclo aún");
         }
     }
     else{
         QMessageBox::critical(this,"data","Seleccionar un registro porfavor");
     }
}

void Registro_horarios::recall(QString movil,QString id){
    QTimer::singleShot(5000, [=](){
            alarm_function(movil, id);
    });
}

//Double click function
void Registro_horarios::on_table_gral_cellDoubleClicked(int row, int column)
{
    qDebug()<<row;
    qDebug()<<column;

     auxiliar_value = "change";
}

void Registro_horarios::on_table_gral_cellClicked(int row, int column)
{
    //Get the position of the clicked cell
    QTableWidgetItem *itab = ui->table_gral->item(row,14);
    QString id = itab->text();

    qDebug()<<column ;

    auxiliar_value="";
    current_id = id;
    current_car = local_movil[current_id]["movil"];
    ui ->label_search -> setText("");
    //qDebug()<<column;

    ui -> frame_movil -> setText(local_movil[current_id]["movil"]);
    ui -> frame_ruta -> setText(local_movil[current_id]["ruta"]);
    ui -> frame_conductor -> setText(local_movil[current_id]["conductor"]);
    ui -> frame_ayudantes -> setText(local_movil[current_id]["ayudantes"]);

    ui -> label_movil -> setText(local_movil[current_id]["movil"]);
    ui -> label_ruta -> setText(local_movil[current_id]["ruta"]);
    ui -> label_conductor -> setText(local_movil[current_id]["conductor"]);
    ui -> label_ayudantes -> setText(local_movil[current_id]["ayudantes"]);
    ui -> text_comentarios -> setPlainText(local_movil[current_id]["comentarios"]);

    //Set the other labels (New model 11/12/2019)
    ui -> salida_base -> setText(local_movil[current_id]["salida_base"]);
    ui -> regreso_base -> setText(local_movil[current_id]["Regreso_base_b"]);
    ui -> inicio_ruta -> setText(local_movil[current_id]["Inicio_ruta_b"]);
    ui -> abandono_ruta -> setText(local_movil[current_id]["Abandono_ruta_b"]);
    ui -> final_ruta -> setText(local_movil[current_id]["Final_ruta_b"]);
    ui -> ingreso_relleno -> setText(local_movil[current_id]["Ingreso_relleno_b"]);
    ui -> salida_relleno -> setText(local_movil[current_id]["Salida_relleno_b"]);
    ui -> inicio_almuerzo -> setText(local_movil[current_id]["Inicio_almuerzo_b"]);
    ui -> final_almuerzo -> setText(local_movil[current_id]["Final_almuerzo_b"]);

    ui -> boton_registrar -> setDisabled(true);
    ui -> button_update -> setEnabled(true);
}

/*
}*/


void Registro_horarios::on_table_gral_cellChanged(int row, int column)
{
    if(auxiliar_value=="change"){
        QString id = ui->table_gral->item(row,14)->text();
        QString reg_change = ui->table_gral->item(row,column)->text();
        QString hour = "";
        QString save_value = "yes";
        QString var;

        if(reg_change.contains(":")){
            QStringList a = reg_change.split("-");
            hour = a[1].replace(" ","");

            a = hour.split(":");
            hour = a[0]+":"+a[1];
            if(a.size()<3){
                hour = "";
            }
        }

        if(hour!=""){
            switch (column) {
            case 0:
                //movil
                // local_movil[id]["movil"] = reg_change;
                //var = "movil";
                auxiliar_value = "";
                update_table(local_movil);
                save_value = "no";
                break;
            case 1:
                //ruta
                //local_movil[id]["ruta"] = reg_change;
                //var = "ruta";
                auxiliar_value = "";
                update_table(local_movil);
                save_value = "no";
                break;
            case 2:
                //conductor
                //local_movil[id]["conductor"] = reg_change;
                //var = "conductor";
                auxiliar_value = "";
                update_table(local_movil);
                save_value = "no";
                break;
            case 3:
                //ayudantes
                //local_movil[id]["ayudantes"] = reg_change;
                //var = "ayudantes";
                auxiliar_value = "";
                update_table(local_movil);
                save_value = "no";
                break;
            case 4:
                //salida_base
                //local_movil[id]["salida_base"] = reg_change;
                local_movil[id]["salida_base"] = reg_change;
                local_movil[id]["salida_base_b"] = hour;
                var = "SalidaBase";
                break;
            case 5:
                //inicioRuta
                local_movil[id]["Inicio_ruta"] = reg_change;
                local_movil[id]["Inicio_ruta_b"] = hour;
                var = "InicioRuta";
                break;
            case 6:
                //final ruta
                local_movil[id]["Final_ruta"] = reg_change;
                local_movil[id]["Final_ruta_b"] = hour;
                var = "FinalRuta";
                break;
            case 7:
                //abandono ruta
                local_movil[id]["Abandono_ruta"] = reg_change;
                local_movil[id]["Abandono_ruta_b"] = hour;
                var = "AbandonoRuta";
                break;
            case 8:
                //ingreso relleno
                local_movil[id]["Ingreso_relleno"] = reg_change;
                local_movil[id]["Ingreso_relleno_b"] = hour;
                var = "IngresoRelleno";
                break;
            case 9:
                //salida relleno
                local_movil[id]["Salida_relleno"] = reg_change;
                local_movil[id]["Salida_relleno_b"] = hour;
                var = "SalidaRelleno";
                break;
            case 10:
                //inicio almuerzo
                local_movil[id]["Inicio_almuerzo"] = reg_change;
                local_movil[id]["Inicio_almuerzo_b"] = hour;
                var = "InicioAlmuerzo";
                break;
            case 11:
                //final almuerzo
                local_movil[id]["Final_almuerzo"] = reg_change;
                local_movil[id]["Final_almuerzo_b"] = hour;
                var = "FinalAlmuerzo";
                break;
            case 12:
                //regreso a base
                local_movil[id]["Regreso_base"] = reg_change;
                local_movil[id]["Regreso_base_b"] = hour;
                var = "RegresoBase";
                break;
            case 13:
                //comentarios
                local_movil[id]["comentarios"] = reg_change;
                var = "comentarios";
                break;
            default:
                break;
            }

            if(save_value =="yes"){
                local_movil[id]["modification"] += "("+var+")";
                save("pendant");
                auxiliar_value = "";
                update_fields();
                update_table(local_movil);
            }
        }
        else{
            if(column!=0&&column!=1&&column!=2&&column!=3){
                 QMessageBox::critical(this,"data","Formato no válido");
            }
            auxiliar_value = "";
            update_table(local_movil);
        }
    }
    auxiliar_value = "";    
}

//This function will search for a relationship between the actual register and the previous registers
QString Registro_horarios::search_relation(QString movil, QString time){

    QHashIterator<QString, QHash <QString, QString>>iterator(local_movil);
    QString id;

    while(iterator.hasNext()){
        auto key = iterator.next().key();

        if(local_movil[key]["movil"]==movil && local_movil[key]["concluded"]=="pendant"){
            id = local_movil[key]["virtual_id"];
            break;
        }
        else{
            id = time;
        }
    }
    return id;
}

void Registro_horarios::search_dependancy(QString movil){

    QHashIterator<QString, QHash<QString, QString>>iterator(local_movil);

    while(iterator.hasNext()){
        auto key = iterator.next().key();

        if(local_movil[key]["movil"]==movil && local_movil[key]["concluded"]=="pendant"){
            local_movil[key]["concluded"] = "concluded";
        }
    }
}

//Saving function
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

void Registro_horarios::saveJson(QHash<QString, QHash<QString,QString>> saver){

    QJsonDocument document;
    QJsonArray main_array;

    //We need to create a virtual id duplicated container
    QStringList saved;

    QHashIterator<QString, QHash<QString, QString>>iter(saver);
    QHashIterator<QString, QHash<QString, QString>>r_iter(db_rutas);

    while(iter.hasNext()){

        auto main_key = iter.next().key();

        if(saved.contains(saver[main_key]["virtual_id"])){

            continue;
        }
        else{
            //Add the gral Object
            QJsonObject main_object;

            main_object.insert("movil",saver[main_key]["movil"]);
            main_object.insert("ruta",saver[main_key]["ruta_id"]);
            main_object.insert("conductor",saver[main_key]["conductor_id"]);
            main_object.insert("ayudantes",saver[main_key]["ayudantes"]);
            main_object.insert("usuario", this->user_name);

            //Now we need to add an Array
            QJsonArray schedule_array;

            QStringList virtual_id = search_same_id(saver[main_key]["virtual_id"],saver);
            saved<<virtual_id;

            foreach (QString item, virtual_id) {
                QJsonObject schedule;

                 //qlonglong stamp = QDateTime::fromString(time, "dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch();

                schedule.insert("salidaBase", QDateTime::fromString(saver[item]["salida_base"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("inicioRuta",QDateTime::fromString(saver[item]["Inicio_ruta"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("finRuta", QDateTime::fromString(saver[item]["Final_ruta"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("abandonoRuta", QDateTime::fromString(saver[item]["Abandono_ruta"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("ingresoRelleno", QDateTime::fromString(saver[item]["Ingreso_relleno"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("salidaRelleno", QDateTime::fromString(saver[item]["Salida_relleno"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("inicioAlmuerzo", QDateTime::fromString(saver[item]["Inicio_almuerzo"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("finalAlmuerzo", QDateTime::fromString(saver[item]["Final_almuerzo"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
                schedule.insert("comentarios", saver[item]["comentarios"]);
                schedule.insert("modificaciones", saver[item]["modification"]);
                schedule.insert("regresoBase", QDateTime::fromString(saver[item]["Regreso_base"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());

                schedule_array.append(schedule);
            }

            main_object.insert("horarios",schedule_array);

            main_array.append(main_object);
        }
    }

    document.setArray(main_array);

    /****************************************************/
    /*****************TO DATABASE*********************/
    /****************************************************/

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);
    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
        QByteArray binReply = reply->readAll ();
        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (binReply);
            if (errorJson.object ().value ("err").toObject ().contains ("message")) {
                QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
            } else {
                QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
            }
            return;
        }
        reply->deleteLater ();

        if(global_session == "yes"){
            emit logOut();
        }
    });

    QNetworkRequest request;
    request.setUrl (QUrl ("http://"+this -> url + "/registroDeHorarios"));
    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");

    nam->post (request, document.toJson ());

    QString path = QDir::homePath();

    QDir any;
    any.mkdir(path+"/LPL_documents");

    QString filename= path+"/LPL_documents/json_horarios.txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(document.toJson());
    file.close();
}

QStringList Registro_horarios::search_same_id(QString cycle_id, QHash<QString,QHash<QString, QString>>saver){

    //search for items with the same virtual ID
    QHashIterator<QString, QHash<QString, QString>>internal_iter(saver);
    QStringList container;

    while(internal_iter.hasNext()){
        auto internal_key = internal_iter.next().key();

        if(saver[internal_key]["virtual_id"] == cycle_id){
            container<<internal_key;
        }
    }

    return container;
}


/***********************************************************************
 **************************DATABASE READING**************************
*************************************************************************/

void Registro_horarios::from_db_readVehicles(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("vehiculos").toArray ()) {

            QHash<QString, QString> current;
            current.insert ("numeroDeAyudantes", QString::number (entidad.toObject ().value ("numeroDeAyudantes").toInt ()));
            current.insert ("movil", entidad.toObject ().value ("movil").toString());
            db_vehiculos.insert (entidad.toObject ().value ("movil").toString (), current);
        }

        //Extracting labels for movil
        QHashIterator<QString, QHash<QString, QString>>movil_iter(db_vehiculos);
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

        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    qDebug()<<this->url;
    request.setUrl (QUrl ("http://"+this->url+"/vehi?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);

}

void Registro_horarios::from_db_readStaff(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("personnel").toArray ()) {

            QHash<QString, QString> current;

            current.insert ("idPersonal", entidad.toObject ().value ("idPersonal").toString());
            current.insert ("nombre", entidad.toObject ().value ("nombre").toString());//TODO --> Change this part

            db_personal.insert(entidad.toObject().value ("idPersonal").toString(), current);

        }

        //Extracting labels for staff
        QHashIterator<QString, QHash<QString, QString>>staff_iter(db_personal);
        QStringList staff_list;

        while(staff_iter.hasNext()){
            staff_list<<db_personal[staff_iter.next().key()]["nombre"];
        }
        std::sort(staff_list.begin(), staff_list.end());

        QCompleter *staff_completer = new QCompleter(staff_list,this);

        staff_completer -> setCaseSensitivity(Qt::CaseInsensitive);
        staff_completer -> setCompletionMode(QCompleter::PopupCompletion);
        staff_completer -> setFilterMode(Qt::MatchContains);
        ui -> label_conductor -> setCompleter(staff_completer);

        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/personnel?from=0&to=10000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

//Link between Vehicles and staff
void Registro_horarios::from_db_readLink_1(){
    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("conductores").toArray ()) {

            QHash<QString, QString> current;

            current.insert ("personal", entidad.toObject ().value ("personal").toString());  //ID PERSONAL
            current.insert ("movil", entidad.toObject ().value ("movil").toString());

            db_link_VP.insert (entidad.toObject ().value ("movil").toString (), current);
        }

        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/conductor?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

//Relationship between Vehicles and routes
void Registro_horarios::from_db_readLink_2(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("vehiculosRutas").toArray ()) {

            QHash<QString, QString> current;

            current.insert ("ruta", QString::number (entidad.toObject ().value ("ruta").toInt ())); // ROUTES ID
            current.insert ("movil", entidad.toObject ().value ("movil").toString());

            db_link_RV.insert (entidad.toObject ().value ("movil").toString (), current);
        }
        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/ruta_vehiculo?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

void Registro_horarios::from_db_readRoutes(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("rutas").toArray ()) {

            QHash<QString, QString> current;
            //qDebug()<<entidad;
            current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ())); // ROUTES ID
            current.insert ("ruta", entidad.toObject ().value ("ruta").toString()); //Route name

            db_rutas.insert(QString::number(entidad.toObject ().value("id").toInt()), current);

        }

        //Extracting labels for routes
        QHashIterator<QString, QHash<QString, QString>>routes_iter(db_rutas);
        QStringList routes_list;

        while(routes_iter.hasNext()){
            routes_list<<db_rutas[routes_iter.next().key()]["ruta"];
        }
        std::sort(routes_list.begin(), routes_list.end());
        QCompleter *routes_completer = new QCompleter(routes_list,this);

        routes_completer -> setCaseSensitivity(Qt::CaseInsensitive);
        routes_completer -> setCompletionMode(QCompleter::PopupCompletion);
        routes_completer -> setFilterMode(Qt::MatchContains);
        ui -> label_ruta -> setCompleter(routes_completer);


        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/ruta?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}


void Registro_horarios::save_data()
{
    global_session = "no";
    QHash<QString, QHash<QString, QString>> db;
    eliminate_data.removeDuplicates();

    foreach (QString item, eliminate_data) {
        if(item!=""){
            done[item] = local_movil[item];
            db[item] = local_movil[item];
            local_movil.remove(item);
        }
    }

    //save("done");
    save("pendant");
    saveJson(db); // This function should send the array to the database
}
