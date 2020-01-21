#include "registro_datos.h"
#include "ui_registro_datos.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>
#include <QCompleter>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QScreen>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

Registro_datos::Registro_datos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_datos)
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

    //Set icons
    double pix_w_b = (width*65)/1920;
    double pix_h_b = (height*65)/1080;

    QPixmap pix_sigma(":/images/img/siremo_verde.png");
    ui->icon_sigma->setPixmap(pix_sigma.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_sigma->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_dato(":/images/img/dato_verde.png");
    ui->icon_dato->setPixmap(pix_dato.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_dato->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_contenedores(":/images/img/contenedor_final_verde.png");
    ui->icon_contenedores->setPixmap(pix_contenedores.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_contenedores->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_comunicacion(":/images/img/radio_verde.png");
    ui->icon_comunicacion->setPixmap(pix_comunicacion.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_comunicacion->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_ejecucion(":/images/img/ejecucion_verde.png");
    ui->icon_ejecucion->setPixmap(pix_ejecucion.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ejecucion->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_verificacion(":/images/img/verificacion_verde.png");
    ui->icon_verificacion->setPixmap(pix_verificacion.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_verificacion->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_conciliacion(":/images/img/conciliacion_verde.png");
    ui->icon_conciliacion->setPixmap(pix_conciliacion.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_conciliacion->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_id(":/images/img/huella_verde.png");
    ui->icon_id->setPixmap(pix_id.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_id->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.38));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.4));

    //adjust the width
     ui -> frame_6 -> setFixedWidth(static_cast<int>(width*0.12));
     ui -> frame_7 -> setFixedWidth(static_cast<int>(width*0.12));
     ui -> frame_15 -> setFixedWidth(static_cast<int>(width*0.12));

     ui -> frame_8 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_9 -> setFixedWidth(static_cast<int>(width*0.15));

     ui -> frame_10 -> setFixedWidth(static_cast<int>(width*0.15));

     ui -> frame_11 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_12 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_13 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_14 -> setFixedWidth(static_cast<int>(width*0.15));

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(timer,SIGNAL(timeout()),this,SLOT(update_counter()));
    timer->start(1000);
    past = 0;

    //Set Search icons
    QPixmap pix_b1(":/images/img/search_2.png");
    QIcon ButtonIcon(pix_b1);
    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));

    /*************
     * Completers*
     *************/
    //Extracting labels for routes

    QStringList containers_list = {
        "3.2 m3",
        "1.5 m3",
        "1.1 m3",
        "10 m3",
        "20 m3",
        "Islas",
    };

    std::sort(containers_list.begin(), containers_list.end());

    QCompleter *container_completer = new QCompleter(containers_list,this);

    container_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    container_completer -> setCompletionMode(QCompleter::PopupCompletion);
    container_completer -> setFilterMode(Qt::MatchContains);
    ui -> label_tipo -> setCompleter(container_completer);

    QStringList maintenance_list = {
        "Grafiteado",
        "Despintado",
        "Abolladura (chapa)",
        "Gomas",
        "Amortiguadores",
        "Agarrador",
        "Cables",
        "Patas",
        "Tapa",
        "Teflon",
        "Llantas/Ruedas",
        "Pedal/Pisaderas",
        "Varillas",
        "Otrs"
    };

    std::sort(maintenance_list.begin(), maintenance_list.end());

    QCompleter *maintenance_completer = new QCompleter(maintenance_list,this);

    maintenance_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    maintenance_completer -> setCompletionMode(QCompleter::PopupCompletion);
    maintenance_completer -> setFilterMode(Qt::MatchContains);
    ui -> label_mantenimiento -> setCompleter(maintenance_completer);

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////WORKING TABLE SIZES//////////////////////////
   ////////////////////////////////////////////////////////////////////////////

    ui -> table_gral -> setColumnCount(21) ;
    ui->table_gral ->setColumnWidth(0,static_cast<int>(width/80));
    ui->table_gral ->setColumnWidth(1,static_cast<int>(width/35));
    ui->table_gral ->setColumnWidth(2,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(3,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(4,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(5,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(6,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(7,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(8,static_cast<int>(width/23));
    ui->table_gral ->setColumnWidth(9,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(10,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(11,static_cast<int>(width/20));
    for(int r=12; r<20; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/19.5));
    }
    ui->table_gral ->setColumnWidth(20,0);

    //Setting the table headers
    QStringList headers = {"Id",
                           "Sigma",
                           "Dato",
                           "Zona",
                           "Dirección",
                           "Cantidad",
                           "Tipo",
                           "Codigo",
                           "Mantenimiento",
                           "Comentarios",
                           "Detalle",
                           "Hora recepción",
                           "Comunicación",
                           "Hora",
                           "Ejecución",
                           "Hora",
                           "Verificación",
                           "Hora",
                           "Conciliación",
                          "Hora"};


    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////CONNECTIONS////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    //connect between the item and the description
    connect(ui->combo_dato,SIGNAL(activated(QString)),this,SLOT(enable()));
    connect(ui->search_item,SIGNAL(clicked()),this,SLOT(enable()));

    //Connection between the searcher and button
    connect(ui->label_search, SIGNAL(returnPressed()),ui->search_item,SLOT(click()));
    connect(ui->label_search,SIGNAL(editingFinished()),ui->search_item,SLOT (click()));


    ui -> combo_dato-> setEditable(true);
    ui -> combo_dato-> setCurrentIndex(-1);
    ui -> combo_dato-> setCurrentText("Seleccionar dato");

}

Registro_datos::~Registro_datos()
{
    delete ui;
}

void Registro_datos::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm:ss");
    ui->label_date->setText(tiempo);
}

void Registro_datos::get_url(QString url){
    this -> url = url;
}

void Registro_datos::get_data(QString real_name, QString user_name, QString token){
    ui->label_user->setText(real_name);

    this -> token = token;
    this -> user_name = user_name;


    //from constructor
    //Set the auxiliar normal by default
    auxiliar = "general";

    //Read temporal files
    read_temporal();
    read_done();

    //read the state of the  counter
    read_counter();

    //update table
    update_table(temporal);

    //clear data in a list
    eliminate_list.clear();

    //Lock by default is true
    lock = false;
    lock_b = true;

    ui -> label_id -> setText(QString::number(counter));

    ui -> button_update -> setDisabled(true);
    ui -> button_respuesta -> setDisabled(true);
    ui -> button_respuesta_2 -> setDisabled(true);
    ui -> button_respuesta_3 -> setDisabled(true);
    ui -> button_respuesta_4 -> setDisabled(true);

    ui -> button_eliminate -> setDisabled(true);

    global_session = "yes";

    on_button_cancel_clicked();

//    from_db_readOverlords();
//    from_db_readDatos();

    //Read all data from a local file
    from_lf_readStaff();
    from_lf_readOverlords();
    from_lf_readDatos();
}

void Registro_datos::enable(){

    QString option = ui-> combo_dato ->currentText();
    QString disabled = "background-color: #EBEDED; "
                                  "border-style: outset; "
                                   "border-radius: 12px; "
                                   "font: 10pt \"MS Shell Dlg 2\"; "
                                   "color: #333333; "
                                   " min-width: 9em; "
                                   "padding: 3px;";
    QString enabled = "background-color: white; "
                                 "border-style: outset; "
                                 "min-height:1.1em;"
                                  "border-radius: 12px; "
                                  "font: 10pt \"MS Shell Dlg 2\"; "
                                  "color: #333333; "
                                  " min-width: 9em; "
                                  "padding: 3px;";

    if ( option == "Poda"){

        //Disable maintenance
        ui -> label_tipo -> setStyleSheet(disabled);
        ui -> label_tipo -> setDisabled(true);
        ui -> label_codigo -> setStyleSheet(disabled);
        ui -> label_codigo -> setDisabled(true);
        ui -> label_mantenimiento -> setStyleSheet(disabled);
        ui -> label_mantenimiento -> setDisabled(true);

        //clean maintenance
        ui -> label_tipo -> setText("");
        ui -> label_codigo -> setText("");
        ui -> label_mantenimiento -> setText("");

        //Enable Poda
        ui -> label_poda -> setStyleSheet(enabled);
        ui -> label_poda -> setDisabled(false);

        //Posterior
        ui -> verificacion -> setStyleSheet(enabled);
        ui ->verificacion -> setDisabled(false);

        ui -> conciliacion -> setStyleSheet(enabled);
        ui ->conciliacion -> setDisabled(false);

        ui ->button_respuesta_3 -> setDisabled(false);
        ui ->button_respuesta_4 -> setDisabled(false);

        auxiliar = "poda";

        ui -> button_respuesta_3 -> setEnabled(true);
        ui -> button_respuesta_4 -> setEnabled(true);

    }
    else if (option == "Mantenimiento de contenedores"){

         //Enable maintenance
        ui -> label_tipo -> setStyleSheet(enabled);
        ui -> label_tipo -> setDisabled(false);
        ui -> label_codigo -> setStyleSheet(enabled);
        ui -> label_codigo -> setDisabled(false);
        ui -> label_mantenimiento -> setStyleSheet(enabled);
        ui -> label_mantenimiento -> setDisabled(false);

        //Disable Poda
        ui -> label_poda -> setStyleSheet(disabled);
        ui -> label_poda -> setDisabled(true);

        //Posterior
        ui -> verificacion -> setStyleSheet(enabled);
        ui ->verificacion -> setDisabled(false);

        ui -> conciliacion -> setStyleSheet(enabled);
        ui ->conciliacion -> setDisabled(false);

        ui ->button_respuesta_3 -> setDisabled(false);
        ui ->button_respuesta_4 -> setDisabled(false);

        //Clean Poda
        ui -> label_poda -> setText("");

        auxiliar = "mantenimiento";
        ui -> button_respuesta_3 -> setEnabled(true);
        ui -> button_respuesta_4 -> setEnabled(true);
    }
    else {

        //Disable maintenance and poda
        ui -> label_tipo -> setStyleSheet(disabled);
        ui -> label_tipo -> setDisabled(true);
        ui -> label_codigo -> setStyleSheet(disabled);
        ui -> label_codigo -> setDisabled(true);
        ui -> label_mantenimiento -> setStyleSheet(disabled);
        ui -> label_mantenimiento -> setDisabled(true);
        ui -> label_poda -> setStyleSheet(disabled);
        ui -> label_poda -> setDisabled(true);

        ui -> verificacion -> setStyleSheet(disabled);
        ui ->verificacion -> setDisabled(true);

        ui -> conciliacion -> setStyleSheet(disabled);
        ui ->conciliacion -> setDisabled(true);

        ui ->button_respuesta_3 -> setDisabled(true);
        ui ->button_respuesta_4 -> setDisabled(true);

        //Clean maintenance and poda
        ui -> label_poda -> setText("");
        ui -> label_tipo -> setText("");
        ui -> label_codigo -> setText("");
        ui -> label_mantenimiento -> setText("");

        auxiliar = "general";
        ui -> button_respuesta_3 -> setDisabled(true);
        ui -> button_respuesta_4 -> setDisabled(true);
    }
}

void Registro_datos::on_button_guardar_clicked()
{
    QString register_id = ui -> label_id -> text();
    QString sigma = ui -> label_sigma -> text();
    QString dato = ui -> combo_dato ->currentText();
    QString zona = ui -> label_zona -> text();
    QString calle = ui -> label_calle -> toPlainText();
    QString detalle = ui -> label_detalle -> text();
    QString comentarios = ui -> text_comentarios -> toPlainText();
    QString cantidad = ui -> label_poda -> text();
    QString tipo = ui -> label_tipo -> text();
    QString codigo = ui -> label_codigo -> text();
    QString mantenimiento = ui -> label_mantenimiento -> text();
    QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");

    QString missing_a = "background-color: #EB403E; "
                                 "border-style: outset; "
                                  "border-radius: 12px; "
                                  "font: 10pt \"MS Shell Dlg 2\"; "
                                  "color: white; "
                                  " min-width: 9em; "
                                  "padding: 3px;";

    QString normal_a = "background-color: white; "
                                        "border-style: outset; "
                                        "border-radius: 12px; "
                                        "font: 10pt \"MS Shell Dlg 2\"; "
                                        "color: #333333; "
                                        " min-width: 9em; "
                                        "padding: 3px;";


    QString missing_b = "background-color: #EB403E; "
                                 "border-style: outset; "
                                  "border-radius: 12px; "
                                  "font: 14pt \"MS Shell Dlg 2\"; "
                                  "color: white; "
                                  "min-width: 2em; "
                                  "max-width: 2em;"
                                  "min-height:1.5em;"
                                  "padding: 3px;";

    QString normal_b = "background-color: white; "
                                 "border-style: outset; "
                                  "border-radius: 12px; "
                                  "font: 14pt \"MS Shell Dlg 2\"; "
                                  "color: #333333; "
                                  "min-width: 2em; "
                                  "max-width: 2em;"
                                  "min-height:1.5em;"
                                  "padding: 3px;";

    ui -> label_sigma -> setStyleSheet(normal_b);
    ui -> combo_dato -> setStyleSheet(normal_a);

    QStringList lista_datos;

    QHashIterator<QString, QHash<QString, QString>>iter_db_datos(db_datos);

    while(iter_db_datos.hasNext()){
        auto d_key = iter_db_datos.next().key();
        lista_datos<<d_key;
    }

    bool selector = false;
    if (lista_datos.contains(dato)){
        selector = true;
    }

     //Lets work with the counter first
   if(register_id!="0"){
        if(register_id.toInt()==0){
            counter--;
        }
        else{
            counter = register_id.toInt();
        }
   }
   else{
        counter = 0;
   }


    if(lock!=true){
        if(sigma !="" && dato!="Seleccionar dato"&&selector==true){
            if(auxiliar == "general"){
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;

                    QHashIterator<QString, QHash<QString, QString>>data_iter(db_datos);

                    while(data_iter.hasNext()){
                        auto this_dato  = data_iter.next().key();

                        if(this_dato == dato ){
                            temporal[time]["dato_id"]=db_datos[this_dato]["id"];
                            break;
                        }
                    }

                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["main_key"]=time;

                    //Correct here
                    temporal[time]["id"]=register_id;
                    temporal[time]["hora"]=time;

                    temporal[time]["cantidad"]="";
                    temporal[time]["tipo"]="";
                    temporal[time]["codigo"]="";
                    temporal[time]["mantenimiento"]="";

                    update_table(temporal);
                    counter++;
                    ui -> label_id ->setText(QString::number(counter));
                    save("pendant");
                    save_counter(counter);

                    restart();
            }
            else if (auxiliar == "poda"){

                if(cantidad!=""){
                    QHashIterator<QString, QHash<QString, QString>>data_iter(db_datos);

                    while(data_iter.hasNext()){
                        auto this_dato  = data_iter.next().key();

                        if(this_dato == dato ){
                            temporal[time]["dato_id"]=db_datos[this_dato]["id"];
                            break;
                        }
                    }

                    ui -> label_poda -> setStyleSheet(normal_a);
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;
                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["id"]=register_id;
                    temporal[time]["hora"]=time;
                    temporal[time]["main_key"]=time;

                    temporal[time]["cantidad"]=cantidad;
                    temporal[time]["tipo"]="";
                    temporal[time]["codigo"]="";
                    temporal[time]["mantenimiento"]="";

                    update_table(temporal);
                    counter++;
                    ui -> label_id ->setText(QString::number(counter));
                    save("pendant");
                    save_counter(counter);

                    restart();
                }
                else{
                    QMessageBox::critical(this,"data","Rellenar los campos obligatorios porfavor");
                    if(cantidad == ""){
                         ui -> label_poda -> setStyleSheet(missing_a);
                    }
                    else{
                        ui -> label_poda -> setStyleSheet(normal_a);
                    }
                }
            }
            else if (auxiliar == "mantenimiento"){
                if(tipo!=""&&mantenimiento!=""){

                    QHashIterator<QString, QHash<QString, QString>>data_iter(db_datos);

                    while(data_iter.hasNext()){
                        auto this_dato  = data_iter.next().key();

                        if(this_dato == dato ){
                            temporal[time]["dato_id"]=db_datos[this_dato]["id"];
                            break;
                        }
                    }

                    ui -> label_mantenimiento -> setStyleSheet(normal_a);
                    ui -> label_codigo -> setStyleSheet(normal_a);
                    ui -> label_tipo -> setStyleSheet(normal_a);
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;
                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["id"]=register_id;
                    temporal[time]["hora"]=time;
                    temporal[time]["main_key"]=time;

                    temporal[time]["cantidad"]="";
                    temporal[time]["tipo"] = tipo;
                    temporal[time]["codigo"] = codigo;
                    temporal[time]["mantenimiento"] = mantenimiento;

                    update_table(temporal);
                    counter++;
                    ui -> label_id ->setText(QString::number(counter));
                    save("pendant");
                    save_counter(counter);

                    restart();
                }
                else{
                        QMessageBox::critical(this,"data","Rellenar los campos obligatorios porfavor");
                        if (tipo==""){
                            ui -> label_tipo -> setStyleSheet(missing_a);
                        }
                        else{
                            ui -> label_tipo -> setStyleSheet(normal_a);
                        }
                        if(codigo==""){
                            //ui -> label_codigo -> setStyleSheet(missing_a);
                        }
                        else{
                            //ui -> label_codigo -> setStyleSheet(normal_a);
                        }
                        if(mantenimiento==""){
                            ui -> label_mantenimiento -> setStyleSheet(missing_a);
                        }
                        else{
                            ui -> label_mantenimiento -> setStyleSheet(normal_a);
                        }
                 }
            }
        }
        else{

               QMessageBox::critical(this,"data","Datos incorrectos");

             if (sigma==""){
                  ui -> label_sigma -> setStyleSheet(missing_b);
             }
             else{
                 ui -> label_sigma -> setStyleSheet(normal_b);
             }

             if(dato=="Seleccionar dato" || selector==false){
                 ui -> combo_dato -> setStyleSheet(missing_a);
             }
             else{
                 ui -> combo_dato -> setStyleSheet(normal_a);
             }
        }
    }
}

void Registro_datos::read_done(){

    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/done_datos.txt";
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
        current.insert("sigma", objetoxd.toObject().value("sigma").toString());
        current.insert("dato", objetoxd.toObject().value("dato").toString());
        current.insert("dato_id", objetoxd.toObject().value("dato_id").toString());
        current.insert("zona", objetoxd.toObject().value("zona").toString());
        current.insert("calle",objetoxd.toObject().value("calle").toString());
        current.insert("detalle",objetoxd.toObject().value("detalle").toString());
        current.insert("cantidad",objetoxd.toObject().value("cantidad").toString());
        current.insert("tipo",objetoxd.toObject().value("tipo").toString());
        current.insert("codigo",objetoxd.toObject().value("codigo").toString());
        current.insert("hora",objetoxd.toObject().value("hora").toString());

        current.insert("mantenimiento",objetoxd.toObject().value("mantenimiento").toString());
        current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());
        current.insert("comunicacion",objetoxd.toObject().value("comunicacion").toString());

        current.insert("ejecucion",objetoxd.toObject().value("ejecucion").toString());
        current.insert("verificacion",objetoxd.toObject().value("verificacion").toString());
        current.insert("conciliacion",objetoxd.toObject().value("conciliacion").toString());

        current.insert("comunicacion_id",objetoxd.toObject().value("comunicacion_id").toString());

        current.insert("ejecucion_id",objetoxd.toObject().value("ejecucion_id").toString());
        current.insert("verificacion_id",objetoxd.toObject().value("verificacion_id").toString());

        current.insert("hora_com",objetoxd.toObject().value("hora_com").toString());
        current.insert("hora_ejec",objetoxd.toObject().value("hora_ejec").toString());
        current.insert("hora_ver",objetoxd.toObject().value("hora_ver").toString());
        current.insert("hora_conc",objetoxd.toObject().value("hora_conc").toString());
        current.insert("main_key",objetoxd.toObject().value("main_key").toString());

        current.insert("id",objetoxd.toObject().value("id").toString());

        done.insert(objetoxd.toObject().value("main_key").toString(),current);
    }
}

void Registro_datos::read_temporal(){

    temporal.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/pendant_datos.txt";
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
        if(objetoxd.toObject().value("main_key").toString()!=""){
            QHash<QString,QString> current;
            current.insert("sigma", objetoxd.toObject().value("sigma").toString());
            current.insert("dato", objetoxd.toObject().value("dato").toString());
            current.insert("dato_id", objetoxd.toObject().value("dato_id").toString());
            current.insert("zona", objetoxd.toObject().value("zona").toString());
            current.insert("calle",objetoxd.toObject().value("calle").toString());
            current.insert("detalle",objetoxd.toObject().value("detalle").toString());
            current.insert("cantidad",objetoxd.toObject().value("cantidad").toString());
            current.insert("tipo",objetoxd.toObject().value("tipo").toString());
            current.insert("codigo",objetoxd.toObject().value("codigo").toString());
            current.insert("hora",objetoxd.toObject().value("hora").toString());

            current.insert("mantenimiento",objetoxd.toObject().value("mantenimiento").toString());
            current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());
            current.insert("comunicacion",objetoxd.toObject().value("comunicacion").toString());

            current.insert("ejecucion",objetoxd.toObject().value("ejecucion").toString());
            current.insert("verificacion",objetoxd.toObject().value("verificacion").toString());
            current.insert("conciliacion",objetoxd.toObject().value("conciliacion").toString());

            current.insert("comunicacion_id",objetoxd.toObject().value("comunicacion_id").toString());

            current.insert("ejecucion_id",objetoxd.toObject().value("ejecucion_id").toString());
            current.insert("verificacion_id",objetoxd.toObject().value("verificacion_id").toString());

            current.insert("hora_com",objetoxd.toObject().value("hora_com").toString());
            current.insert("hora_ejec",objetoxd.toObject().value("hora_ejec").toString());
            current.insert("hora_ver",objetoxd.toObject().value("hora_ver").toString());
            current.insert("hora_conc",objetoxd.toObject().value("hora_conc").toString());
            current.insert("main_key",objetoxd.toObject().value("main_key").toString());

            current.insert("id",objetoxd.toObject().value("id").toString());

            temporal.insert(objetoxd.toObject().value("main_key").toString(),current);
        }
    }
}

void Registro_datos::save(QString action){

    QJsonDocument documentoxd;
    QJsonObject datosxd;
    QJsonArray arrayDeDatos;
    QHash<QString, QHash<QString, QString>>saver;
    if(action == "pendant"){
        saver = temporal;
    }
    else{
        saver = done;
    }

    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();
        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;
        currnt.insert("hora",item);
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

    QString filename= path+"/LPL_documents/"+action+"_datos.txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(documentoxd.toJson());
    file.close();
}

void Registro_datos::save_counter(int cnt){

    QString time = QDateTime::currentDateTime().toString("hh:mm");
    QStringList splitted = time.split(":");


    QString path = QDir::homePath();
    QDir any;
    any.mkdir(path+"/LPL_documents");
    QString filename= path+"/LPL_documents/id_register.txt";
    QFile file(filename );

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream( &file );
        stream<< cnt <<";"<<splitted[0]<<";"<<splitted[1]<< endl;
    }
    file.close();
}


//We need to change the counter for the ID, some data need to be replaced with QTC, so this will be a new Field
void Registro_datos::read_counter(){

     //Solve the counter enigma
     //IDEA: Put the time in the register
     //    QString time = QDateTime::currentDateTime().toString("hh:mm");
     //    QStringList split_time = time.split(":");

    QString path = QDir::homePath();

    QFile file(path+"/LPL_documents/id_register.txt");

    QString line;
    QStringList split_data;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line = stream.readLine();
        }
    }

    split_data = line.split(";");

    QString actual_time = QDateTime::currentDateTime().toString("hh:mm");
    QStringList spplitTime = actual_time.split(":");

    //REVISAR ESTE CAMPO
//    if (spplitTime[0] >= 6 && split_data[0] < 6){
//        counter = 0;
//    }
//    else{
//        if(split_data[0].toInt()<6000){
//            counter = split_data[0].toInt();
//        }
//        else{
//            counter = 0;
//        }
//    }

    counter = split_data[0].toInt();

    file.close();
}

void Registro_datos::update_table(QHash<QString, QHash<QString,QString>>update){

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

       // QDateTime datetime = QDateTime::fromString(dateString, "dd.MM.yyyy - HH:mm:ss" );

        //Writing the current row
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(update[current]["id"]));
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["sigma"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["dato"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["zona"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["calle"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["cantidad"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["tipo"]));
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["codigo"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["mantenimiento"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(update[current]["detalle"]));
        ui->table_gral->setItem(row_control, 10, new QTableWidgetItem(update[current]["comentarios"]));
        ui->table_gral->setItem(row_control, 11, new QTableWidgetItem(update[current]["hora"]));
        ui->table_gral->setItem(row_control, 12, new QTableWidgetItem(update[current]["comunicacion"]));
        ui->table_gral->setItem(row_control, 13, new QTableWidgetItem(update[current]["hora_com"]));
        ui->table_gral->setItem(row_control, 14, new QTableWidgetItem(update[current]["ejecucion"]));
        ui->table_gral->setItem(row_control, 15, new QTableWidgetItem(update[current]["hora_ejec"]));
        ui->table_gral->setItem(row_control, 16, new QTableWidgetItem(update[current]["verificacion"]));
        ui->table_gral->setItem(row_control, 17, new QTableWidgetItem(update[current]["hora_ver"]));
        ui->table_gral->setItem(row_control, 18, new QTableWidgetItem(update[current]["conciliacion"]));
        ui->table_gral->setItem(row_control, 19, new QTableWidgetItem(update[current]["hora_conc"]));
        ui->table_gral->setItem(row_control, 20, new QTableWidgetItem(update[current]["main_key"]));


        if(update[current]["dato"]=="Mantenimiento de contenedores" || update[current]["dato"]=="Poda"){
            if(update[current]["conciliacion"]!=""){
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
                ui->table_gral->item(row_control,14)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,15)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,16)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,17)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,18)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,19)->setBackground(QColor("#B7E1DF"));
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
                ui->table_gral->item(row_control,14)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,15)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,16)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,17)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,18)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,19)->setBackground(QColor("#FFFFFF"));
            }
        }
        else{
            if(update[current]["ejecucion"]!=""){
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
                ui->table_gral->item(row_control,14)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,15)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,16)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,17)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,18)->setBackground(QColor("#B7E1DF"));
                ui->table_gral->item(row_control,19)->setBackground(QColor("#B7E1DF"));
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
                ui->table_gral->item(row_control,14)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,15)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,16)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,17)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,18)->setBackground(QColor("#FFFFFF"));
                ui->table_gral->item(row_control,19)->setBackground(QColor("#FFFFFF"));
            }
        }


    }
    ui->table_gral->setSortingEnabled(true);
    ui->table_gral->sortByColumn(0,Qt::AscendingOrder);
}


//This function is going to be a Filter now
void Registro_datos::on_search_item_clicked()
{
    QString search = ui -> label_search -> text();
    QString actual_value = ui->combo_dato->currentText();

    int counter = 0;
    //Search for the register in the table
    QHashIterator<QString, QHash<QString, QString>>iter(temporal);
    QHash<QString, QHash<QString, QString>>local;

    while(iter.hasNext()){
        auto actual = iter.next().key();
        if (temporal[actual]["id"]==search){
            counter = 1;
            local[actual]=temporal[actual];
        }
    }

    if(counter != 0){

        update_table(local);

    }
    else{
         QMessageBox::critical(this,"data","Dato no registrado");
    }
}

//TODO RESET ALLAFTER SAVING SOMETHING
void Registro_datos::on_button_respuesta_clicked()
{
    QString current = ui -> comunicacion -> text();
    QString time = ui -> label_date -> text();
    if(current_id!=""){
        if(current!=""){

            QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);

            while(iter_staff.hasNext()){
                    auto staff_id = iter_staff.next().key();

                    if(db_personal[staff_id]["nombre"] == current){
                           temporal[current_id]["comunicacion_id"] = staff_id;
                           break;
                    }
            }


            temporal[current_id]["comunicacion"] = current; // personal
            temporal[current_id]["hora_com"] = time;

            update_table(temporal);
            save("pendant");
            restart();

        }
        else{
            QMessageBox::critical(this,"data","Ingresar campos porfavor");
        }
    }
}

void Registro_datos::on_button_respuesta_2_clicked()
{
    QString current = ui -> ejecucion -> text();
    QString time = ui -> label_date -> text();

    if(current!=""){
        if(auxiliar!="general"){

            QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);

            while(iter_staff.hasNext()){
                    auto staff_id = iter_staff.next().key();

                    if(db_personal[staff_id]["nombre"] == current){
                           temporal[current_id]["ejecucion_id"] = staff_id;
                           break;
                    }
            }

            temporal[current_id]["ejecucion"]=current;
            temporal[current_id]["hora_ejec"]=time;
            update_table(temporal);
            save("pendant");
            restart();

        }
        else{

            QHashIterator<QString, QHash<QString, QString>> iter_staff(db_personal);

            while(iter_staff.hasNext()){
                    auto staff_id = iter_staff.next().key();

                    if(db_personal[staff_id]["nombre"] == current){
                           temporal[current_id]["ejecucion_id"] = staff_id;
                           break;
                    }
            }

            temporal[current_id]["ejecucion"]=current;
            temporal[current_id]["hora_ejec"]=time;
            //done[current_id] = temporal[current_id];

            update_table(temporal);
            eliminate_list<<current_id;

            save("pendant");
            //save("done");
            restart();

        }
    }
    else{
        QMessageBox::critical(this,"data","Ingresar campos porfavor");
    }
}

void Registro_datos::on_button_respuesta_4_clicked()
{
    QString current = ui -> verificacion -> text();
    QString time = ui -> label_date -> text();
    QHashIterator<QString,QHash<QString, QString>>o_iter(db_overlords);

    if(current!=""){
        if(auxiliar!="general"){

            while(o_iter.hasNext()){
                auto o_key = o_iter.next().key();

                if(o_key==current){
                    temporal[current_id]["verificacion_id"] = db_overlords[o_key]["id"];
                    break;
                }
            }

            temporal[current_id]["verificacion"]=current;
            temporal[current_id]["hora_ver"]=time;
            update_table(temporal);

            save("pendant");
            restart();

        }
        else{
            QMessageBox::critical(this,"data","Este campo no es requerido");
        }
    }
    else{
        QMessageBox::critical(this,"data","Ingresar campos porfavor");
    }

}

void Registro_datos::on_button_respuesta_3_clicked()
{
    QString current = ui -> conciliacion -> text();
    QString time = ui -> label_date -> text();

    if(current!=""){
        if(auxiliar!="general"){
            temporal[current_id]["conciliacion"]=current;
            temporal[current_id]["hora_conc"]=time;
            update_table(temporal);
            eliminate_list<<current_id;

            save("pendant");
            //save("done");
            restart();
            lock = false;
            lock_b = true;
        }
        else{
            QMessageBox::critical(this,"data","Este campo no es requerido");
        }
    }
    else{
        QMessageBox::critical(this,"data","Ingresar campos porfavor");
    }
}

void Registro_datos::on_close_button_clicked()
{
    emit close_all();

    QHash<QString,QHash<QString,QString>>db = done;

    eliminate_list.removeDuplicates();

    foreach (QString val, eliminate_list) {
        done [val] = temporal [val];
        db[val] = temporal [val];
        temporal.remove(val);
    }
    save("pendant");
    //save("done");
    saveJson(db);

}

QString Registro_datos::search_dato(QString item){
    //Start a QIterator for the local movil where we are searching the movil
    QHashIterator<QString, QHash<QString, QString>>iter(temporal);
    QString key_search;

    while(iter.hasNext()){

        auto actual = iter.next().key();
        if(temporal[actual]["numero_dato"]==item){
            key_search = actual;
            break;
        }
    }
    return key_search;
}

//VERIFY INTEGRITY OF THIS FUNCTION
void Registro_datos::update_counter(){
    QString actual_time = QDateTime::currentDateTime().toString("hh:mm");
    QStringList splitted = actual_time.split(":");

    if(splitted[0].toInt() == 6 && past == 5){
        counter = 0;
    }
    past = splitted[0].toInt();
}

void Registro_datos::restart(){

    ui -> label_id -> setText(QString::number(counter));
    ui -> label_sigma -> setText("");
    ui -> combo_dato -> setCurrentText("Seleccionar dato");
    ui -> label_zona -> setText("");
    ui -> label_calle -> setText("");

    ui -> label_detalle -> setText("");
    ui -> text_comentarios -> setPlainText("");

    ui -> label_poda -> setText("");
    ui -> label_tipo -> setText("");
    ui -> label_codigo -> setText("");
    ui -> label_mantenimiento -> setText("");

    ui->comunicacion->setText("");
    ui->ejecucion->setText("");
    ui->conciliacion->setText("");
    ui->verificacion->setText("");

    ui -> button_guardar -> setEnabled(true);
    ui -> button_update -> setDisabled(true);
    ui -> button_eliminate -> setDisabled(true);

    ui -> button_respuesta -> setDisabled(true);
    ui -> button_respuesta_2 -> setDisabled(true);
    ui -> button_respuesta_3 -> setDisabled(true);
    ui -> button_respuesta_4 -> setDisabled(true);

    current_id = "";

}

//This function should erase everything from the saving
void Registro_datos::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        restart();
    }
}

void Registro_datos::on_restart_clicked()
{
    update_table(temporal);
}

//
void Registro_datos::on_table_gral_cellClicked(int row, int column)
{
    //Get the value of the clicked cell
    current_id = ui->table_gral->item(row,20)-> text();
    qDebug()<<column;

    ui -> label_id -> setText(temporal[current_id]["id"]);
    ui -> label_sigma -> setText(temporal[current_id]["sigma"]);
    ui -> combo_dato ->setCurrentText(temporal[current_id]["dato"]);
    ui -> label_zona -> setText(temporal[current_id]["zona"]);
    ui -> label_calle -> setText(temporal[current_id]["calle"]);

    ui -> label_detalle -> setText(temporal[current_id]["detalle"]);
    ui -> text_comentarios -> setPlainText(temporal[current_id]["comentarios"]);

    ui -> label_poda -> setText(temporal[current_id]["cantidad"]);
    ui -> label_tipo -> setText(temporal[current_id]["tipo"]);
    ui -> label_codigo -> setText(temporal[current_id]["codigo"]);
    ui -> label_mantenimiento -> setText(temporal[current_id]["mantenimiento"]);

    ui->comunicacion->setText(temporal[current_id]["comunicacion"]);
    ui->ejecucion->setText(temporal[current_id]["ejecucion"]);
    ui->conciliacion->setText(temporal[current_id]["conciliacion"]);
    ui->verificacion->setText(temporal[current_id]["verificacion"]);

    ui -> button_guardar -> setDisabled(true);
    ui -> button_update -> setEnabled(true);
    ui -> button_eliminate -> setEnabled(true);

    ui -> button_respuesta -> setEnabled(true);
    ui -> button_respuesta_2 -> setEnabled(true);
    ui -> button_respuesta_3 -> setEnabled(true);
    ui -> button_respuesta_4 -> setEnabled(true);

    enable();
}

void Registro_datos::on_button_cancel_clicked()
{
    restart();
}

void Registro_datos::on_button_eliminate_clicked()
{
    if (current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            temporal.remove(current_id);
            eliminate_list.removeOne(current_id);
            update_table(temporal);
            save("pendant");
            restart();
        }
    }
}

void Registro_datos::on_button_update_clicked()
{
    if(current_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Actualización", "Seguro desea modificar los datos de este registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            temporal[current_id]["id"] = ui -> label_id -> text();
            temporal[current_id]["sigma"] = ui -> label_sigma -> text();
            temporal[current_id]["dato"] = ui -> combo_dato ->currentText();
            temporal[current_id]["zona"] = ui -> label_zona -> text();
            temporal[current_id]["calle"] = ui -> label_calle -> toPlainText();

            temporal[current_id]["detalle"] = ui -> label_detalle -> text();
            temporal[current_id]["comentarios"] = ui -> text_comentarios -> toPlainText();

            temporal[current_id]["cantidad"] = ui -> label_poda -> text();
            temporal[current_id]["tipo"] = ui -> label_tipo -> text();
            temporal[current_id]["codigo"] = ui -> label_codigo -> text();
            temporal[current_id]["mantenimiento"] = ui -> label_mantenimiento -> text();

            restart();
            update_table(temporal);
            save("pendant");
        }
    }
}

void Registro_datos::from_db_readOverlords(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);
        QHash<QString, QHash<QString, QString>>temporal;

        foreach (QJsonValue entidad, okJson.object ().value ("supervisores").toArray ()) {

            QHash<QString, QString> current;
            current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ()));
            current.insert ("zona", entidad.toObject ().value ("zona").toString());

            temporal.insert(entidad.toObject ().value("zona").toString(), current);
        }

        file_writing(temporal,"supervisors.txt");

        from_lf_readDatos();
        from_lf_readStaff();
        from_lf_readOverlords();

        emit send_update();

        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/supervisor?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

void Registro_datos::saveJson(QHash<QString, QHash<QString, QString>>saver){
    QJsonDocument document;
    QJsonArray main_array;

    //We need to create a virtual id duplicated container
    QStringList saved;

    QHashIterator<QString, QHash<QString, QString>>iter(saver);

    while(iter.hasNext()){
        auto main_key = iter.next().key();

        QJsonObject main_object;

        main_object.insert("idDiario", saver[main_key]["id"]);
        main_object.insert("sigmaDeRecepcion", saver[main_key]["sigma"]);

        main_object.insert("horaDeRecepcion",QDateTime::fromString(saver[main_key]["hora"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());

        main_object.insert("zona", saver[main_key]["zona"]);
        main_object.insert("direccion", saver[main_key]["calle"]);
        main_object.insert("cantidadPoda", saver[main_key]["cantidad"]);
        main_object.insert("detalle", saver[main_key]["detalle"]);
        main_object.insert("comentarios", saver[main_key]["comentarios"]);
        main_object.insert("tipoDeContenedor", saver[main_key]["tipo"]);
        main_object.insert("codigoDeContenedor", saver[main_key]["codigo"]);
        main_object.insert("Mantenimiento", saver[main_key]["mantenimiento"]);

        main_object.insert("horaComunicacion",QDateTime::fromString(saver[main_key]["hora_com"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("horaEjecucion",QDateTime::fromString(saver[main_key]["hora_ejec"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("horaVerificacion",QDateTime::fromString(saver[main_key]["hora_ver"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("horaConciliacion",QDateTime::fromString(saver[main_key]["hora_conc"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());

        main_object.insert("sigmaDeConciliacion", saver[main_key]["conciliacion"]);
        main_object.insert("dato", saver[main_key]["dato_id"].toInt());
        main_object.insert("responsableComunicacion", saver[main_key]["comunicacion_id"]);
        main_object.insert("responsableEjecucion", saver[main_key]["ejecucion_id"]);
        main_object.insert("supervisor", saver[main_key]["verificacion_id"].toInt());

        main_object.insert("usuario_id", this -> user_name);

        main_array.append(main_object);
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
                save("done");
                if(global_session=="yes"){
                    emit logOut();
                }
            }
        }
        else{
            done.clear();
            save("done");
            if(global_session=="yes"){
                emit logOut();
            }
        }
      reply->deleteLater ();
     });

    QNetworkRequest request;
    request.setUrl (QUrl ("http://"+this -> url + "/registroDeDatos"));
    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");

    nam->post (request, document.toJson ());
}

void Registro_datos::on_comunicacion_editingFinished()
{
    QString current_text = ui -> comunicacion -> text();

    QHashIterator<QString, QHash<QString, QString>>iter(db_personal);
    QString flag = "not";

    if(current_text != ""){
        while(iter.hasNext()){
            auto search = iter.next().key();
            if(db_personal[search]["nombre"]==current_text){
                flag = "yes";
            }
        }
        if(flag == "not"){
            QMessageBox::critical (this, "Error", "Supervisor inexistente");
            ui -> comunicacion -> setText("");
        }
        else{
             ui -> comunicacion -> setText(current_text);
        }
    }
}

void Registro_datos::on_ejecucion_editingFinished()
{
    QString current_text = ui -> ejecucion -> text();

    QHashIterator<QString, QHash<QString, QString>>iter(db_personal);
    QString flag = "not";

    if(current_text != ""){
        while(iter.hasNext()){
            auto search = iter.next().key();
            if(db_personal[search]["nombre"]==current_text){
                flag = "yes";
            }
        }
        if(flag == "not"){
            QMessageBox::critical (this, "Error", "Supervisor inexistente");
            ui -> ejecucion -> setText("");
        }
        else{
             ui -> ejecucion -> setText(current_text);
        }
    }
}

void Registro_datos::on_verificacion_editingFinished()
{
    QString current_text = ui -> verificacion -> text();

    QHashIterator<QString, QHash<QString, QString>>iter(db_overlords);
    QString flag = "not";

    if(current_text != ""){
        while(iter.hasNext()){
            auto search = iter.next().key();
            if(search==current_text){
                flag = "yes";
            }
        }
        if(flag == "not"){
            QMessageBox::critical (this, "Error", "Supervisor inexistente");
            ui -> verificacion -> setText("");
        }
        else{
             ui -> verificacion -> setText(current_text);
        }
    }
}

void Registro_datos::save_data(){

    global_session = "no";
    QHash<QString,QHash<QString,QString>>db=done;

    eliminate_list.removeDuplicates();

    foreach (QString val, eliminate_list) {
        done [val] = temporal [val];
        db[val] = temporal [val];
        temporal.remove(val);
    }
    save("pendant");
    //save("done");
    saveJson(db);
}

void Registro_datos::from_db_readDatos(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);
        QHash<QString, QHash<QString, QString>>temporal;

        foreach (QJsonValue entidad, okJson.object ().value ("listaDeDatos").toArray ()) {

            QHash<QString, QString> current;
            current.insert ("id", QString::number(entidad.toObject ().value ("id").toInt()));
            current.insert ("dato", entidad.toObject ().value ("dato").toString());//TODO --> Change this part
            temporal.insert(entidad.toObject().value ("dato").toString(), current);
        }

        file_writing(temporal, "datos.txt");
        from_db_readOverlords();
        reply->deleteLater ();

    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/listaDeDatos_OR?"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

void Registro_datos::from_lf_readStaff()
{
    db_personal.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/db_files/staff.txt";
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
        current.insert ("idPersonal", object.toObject ().value ("idPersonal").toString());
        current.insert ("nombre", object.toObject ().value ("nombre").toString());

        db_personal.insert(object.toObject().value("idPersonal").toString(),current);
    }

    //Extracting labels for routes
    QHashIterator<QString, QHash<QString, QString>>staff_iter(db_personal);
    QStringList staff_list;

    while(staff_iter.hasNext()){
        staff_list<<db_personal[staff_iter.next().key()]["nombre"];
    }
    std::sort(staff_list.begin(), staff_list.end());

    QCompleter *super_completer = new QCompleter(staff_list,this);

    super_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    super_completer -> setCompletionMode(QCompleter::PopupCompletion);
    super_completer -> setFilterMode(Qt::MatchContains);
    ui -> comunicacion -> setCompleter(super_completer);
    ui -> ejecucion -> setCompleter(super_completer);

}

void Registro_datos::from_lf_readOverlords()
{
    db_overlords.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/db_files/supervisors.txt";
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
        current.insert ("zona", object.toObject ().value ("zona").toString());

        db_overlords.insert(object.toObject ().value("zona").toString(), current);
    }

    //Extracting labels for routes
    QHashIterator<QString, QHash<QString, QString>>sup_iter(db_overlords);
    QStringList sup_list;

    while(sup_iter.hasNext()){
        sup_list<<sup_iter.next().key();
    }
    std::sort(sup_list.begin(), sup_list.end());

    QCompleter *super_completer = new QCompleter(sup_list,this);

    super_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    super_completer -> setCompletionMode(QCompleter::PopupCompletion);
    super_completer -> setFilterMode(Qt::MatchContains);
    ui -> verificacion -> setCompleter(super_completer);
}

void Registro_datos::from_lf_readDatos()
{
    db_datos.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/db_files/datos.txt";
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
        current.insert ("dato", object.toObject ().value ("dato").toString());

        db_datos.insert(object.toObject().value ("dato").toString(), current);
    }

    //Extracting labels for routes
    QHashIterator<QString, QHash<QString, QString>>dato_iter(db_datos);
    QStringList datos_list;

    while(dato_iter.hasNext()){
        datos_list << dato_iter.next().key();
    }

    std::sort(datos_list.begin(), datos_list.end());

    foreach (QString itm, datos_list){
            ui -> combo_dato -> addItem(itm);
     }
}

void Registro_datos::file_writing(QHash<QString, QHash<QString, QString>>saver, QString json)
{
    QJsonDocument document;
    QJsonArray array;
    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();

        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;

        while(it_2.hasNext()){
            auto valores=it_2.next();
            currnt.insert(valores.key(),valores.value());
        }

        array.append(currnt);
     }

    document.setArray(array);
    QString path = QDir::homePath();

    QDir any;
    any.mkdir(path+"/LPL_documents/db_files");
    QString filename= path+"/LPL_documents/db_files/"+json;

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(document.toJson());
    file.close();
}

void Registro_datos::update_data(){
    from_db_readDatos();
}
