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
#include <QDir>
#include <QCompleter>
#include <QScreen>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSound>

Registro_penalidades::Registro_penalidades(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_penalidades)
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

    double pix_w_b = (width*60)/1920;
    double pix_h_b = (height*60)/1080;

    QPixmap pix_sigma(":/images/img/siremo_verde.png");
    QPixmap pix_supervisor(":/images/img/encargadoLPL_verde.png");
    QPixmap pix_ruta(":/images/img/ruta-verde.png");
    QPixmap pix_movil(":/images/img/movil_verde.png");

    ui->icon_sigma->setPixmap(pix_sigma.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_sigma->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    ui->icon_supervisor->setPixmap(pix_supervisor.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_supervisor->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    ui->icon_ruta->setPixmap(pix_ruta.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ruta->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    ui->icon_movil->setPixmap(pix_movil.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_movil->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));


    //Set the table Size
    ui ->table_gral ->setColumnCount(15);
    ui->table_gral ->setColumnWidth(0,static_cast<int>(width/40));
    ui->table_gral ->setColumnWidth(1,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(2,static_cast<int>(width/40));
    ui->table_gral ->setColumnWidth(3,static_cast<int>(width/25));
    ui->table_gral ->setColumnWidth(4,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(5,static_cast<int>(width/8));
    ui->table_gral ->setColumnWidth(6,static_cast<int>(width/15));
    ui->table_gral ->setColumnWidth(7,static_cast<int>(width/4.6));
    ui->table_gral ->setColumnWidth(8,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(13,static_cast<int>(width/10.2));

    for(int r=9; r<12; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/17.25));
    }
    ui->table_gral ->setColumnWidth(14,0);

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.32));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.45));

    //adjust the width
     ui -> frame_11 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_12 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_13 -> setFixedWidth(static_cast<int>(width*0.18));
     ui -> frame_6 -> setFixedWidth(static_cast<int>(width*0.18));

     ui -> frame_14 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_15 -> setFixedWidth(static_cast<int>(width*0.1));
     ui -> frame_16 -> setFixedWidth(static_cast<int>(width*0.15));
     ui -> frame_17 -> setFixedWidth(static_cast<int>(width*0.15));

//     ui -> frame_8 -> setFixedWidth(static_cast<int>(width*0.1));

//     ui -> frame_13 -> setFixedWidth(static_cast<int>(width*0.15));

//     ui -> frame_18 -> setFixedWidth(static_cast<int>(width*0.15));

    //Set Search icons
    QPixmap pix_b1(":/images/img/search_2.png");
    QIcon ButtonIcon(pix_b1);
    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));
    ui->search_sigma->setIcon(ButtonIcon);
    ui->search_sigma->setIconSize(QSize(20,20));

    //Setting the table headers
    QStringList headers = {"Item",
                           "Tipo de Penalidad",
                           "Sigma",
                           "Ruta",
                           "Movil",
                           "Detalle",
                           "Hora recepción",
                           "Descripción",
                           "Supervisor",
                           "Respuesta",
                           "Hora Respuesta",
                           "Contra Respuesta",
                           "Hora Contrarespuesta",
                           "Comentarios"};

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

    //Save A Hash from the Json File
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

    //connect the filters
    connect(ui->label_sigmasearh, SIGNAL(returnPressed()),ui->search_sigma,SLOT(click()));
    connect(ui->label_itemsearch, SIGNAL(returnPressed()),ui->search_item,SLOT(click()));

    //Set the acutal table by default
    actual_table = "general";

    //read the Json to retreive the data
    QString content;
    QString path = QDir::homePath();
    QString file_name = path+"/LPL_documents/pendant.txt";
    QFile file_2 (file_name);

    if(!file_2.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }

    else{
        content = file_2.readAll();
        file_2.close();
    }

    QJsonDocument documento = QJsonDocument::fromJson(content.toUtf8());
    QJsonArray arraydatos = documento.array();

    foreach(QJsonValue object, arraydatos){

        QHash<QString,QString> current;

        current.insert("item", object.toObject().value("item").toString());
        current.insert("tipo", object.toObject().value("tipo").toString());
        current.insert("ruta", object.toObject().value("ruta").toString());
        current.insert("ruta_id", object.toObject().value("ruta_id").toString());
        current.insert("movil",object.toObject().value("movil").toString());
        current.insert("detalle",object.toObject().value("detalle").toString());
        current.insert("recepcion",object.toObject().value("recepcion").toString());

        current.insert("sigma",object.toObject().value("sigma").toString());
        current.insert("supervisor",object.toObject().value("supervisor").toString());
        current.insert("supervisor_id",object.toObject().value("supervisor_id").toString());
        current.insert("respuesta",object.toObject().value("respuesta").toString());

        current.insert("hora_respuesta",object.toObject().value("hora_respuesta").toString());
        current.insert("contra",object.toObject().value("contra").toString());
        current.insert("hora_contra",object.toObject().value("hora_contra").toString());

        current.insert("descripcion",object.toObject().value("descripcion").toString());
        current.insert("comentarios",object.toObject().value("comentarios").toString());
        current.insert("id",object.toObject().value("id").toString());

        local_item.insert(object.toObject().value("id").toString(),current);

    }

    update_table(local_item);

    //Now read the done data
    QString content_done;
    QString done = path+"/LPL_documents/done.txt";
    QFile file_done (done);

    if(!file_done.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo";
    }

    else{
        content = file_done.readAll();
        file_done.close();
    }

    QJsonDocument done_document = QJsonDocument::fromJson(content.toUtf8());
    QJsonArray done_array = done_document.array();

    foreach(QJsonValue object, done_array){

        QHash<QString,QString> current;

        current.insert("item", object.toObject().value("item").toString());
        current.insert("tipo", object.toObject().value("tipo").toString());
        current.insert("ruta", object.toObject().value("ruta").toString());
        current.insert("movil",object.toObject().value("movil").toString());
        current.insert("detalle",object.toObject().value("detalle").toString());
        current.insert("recepcion",object.toObject().value("recepcion").toString());

        current.insert("sigma",object.toObject().value("sigma").toString());
        current.insert("supervisor",object.toObject().value("supervisor").toString());
        current.insert("respuesta",object.toObject().value("respuesta").toString());

        current.insert("hora_respuesta",object.toObject().value("hora_respuesta").toString());
        current.insert("contra",object.toObject().value("contra").toString());
        current.insert("hora_contra",object.toObject().value("hora_contra").toString());
        current.insert("descripcion",object.toObject().value("descripcion").toString());
        current.insert("comentarios",object.toObject().value("comentarios").toString());
        current.insert("id",object.toObject().value("id").toString());

        local_done.insert(object.toObject().value("id").toString(),current);
    }

    //Completer for penaltie type

    QStringList penalties = {"Infraccion", "Deficiencia"};

    QCompleter *penalties_completer = new QCompleter(penalties,this);

    penalties_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    penalties_completer -> setCompletionMode(QCompleter::PopupCompletion);
    penalties_completer -> setFilterMode(Qt::MatchContains);
    ui -> label_penalidad -> setCompleter(penalties_completer);

    //Initialize data to remove
    eliminate_data.clear();

    //Set the buttons of answer by default locked
    ui -> button_respuesta -> setDisabled(true);
    ui -> butto_contrarespuesta -> setDisabled(true);
    ui -> button_update->setDisabled(true);

    global_session = "yes";
}

Registro_penalidades::~Registro_penalidades()
{
    delete ui;
}

void Registro_penalidades::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd/MM/yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->label_date->setText(tiempo);
}

void Registro_penalidades::get_url(QString url){
    this -> url = url;
}

void Registro_penalidades::get_data(QString real_name, QString user_name, QString token){
    ui->label_user->setText(real_name);
    this -> user_name = user_name;
    this -> token = token;
    from_db_readStaff();
    from_db_readVehicles();
    from_db_readLink_2();
    from_db_readLink_1();
    from_db_readRoutes();
    from_db_readOverlords();
}

void Registro_penalidades::set_description(){
    QString actual_item = ui->label_item->text();
    if(actual_item!=""){
        if(penalidades[actual_item]["Detalle"]!=""){
            ui -> label_description -> setText(penalidades[actual_item]["Detalle"]);
            ui -> label_penalidad -> setText(penalidades[actual_item]["Tipo"]);
        }
        else{
            ui->label_description->setText("");
            ui->label_penalidad->setText("");
            ui->label_item->setText("");
            QMessageBox::critical(this,"data","Item Inexistente");
        }
    }
}

void Registro_penalidades::read_vehicles(){

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

void Registro_penalidades::read_staff(){
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

void Registro_penalidades::on_button_guardar_clicked()
{
    //This button sends the data to the local Hash and shows it in the table
    QString sigma = ui -> label_sigma -> text();
    QString tipo = ui -> label_penalidad -> text();
    QString ruta = ui -> label_ruta -> text();
    QString movil = ui -> label_movil -> text();
    QString item = ui -> label_item -> text();
    QString detalle = ui -> label_detalle -> toPlainText();
    QString recepcion = ui -> label_date -> text();
    QString supervisor = ui -> supervisor_1 ->text();

    QHashIterator<QString,QHash<QString, QString>>r_iter(db_rutas);
    QHashIterator<QString,QHash<QString, QString>>o_iter(db_overlords);

    if(tipo=="Infraccion"||tipo=="Deficiencia"){
        if(sigma!="" && tipo!=""  && item!=""){

                    while(r_iter.hasNext()){
                        auto v_key = r_iter.next().key();

                        if(db_rutas[v_key]["ruta"]==ruta){
                            local_item[recepcion]["ruta_id"] = v_key;
                            break;
                        }
                    }

                    while(o_iter.hasNext()){
                        auto o_key = o_iter.next().key();

                        if(o_key==supervisor){
                            local_item[recepcion]["supervisor_id"] = db_overlords[o_key]["id"];
                            break;
                        }
                    }

                    local_item[recepcion]["item"] = item;
                    local_item[recepcion]["tipo"] = tipo;
                    local_item[recepcion]["ruta"] = ruta;
                    local_item[recepcion]["movil"] = movil;
                    local_item[recepcion]["detalle"] = detalle;
                    local_item[recepcion]["recepcion"] = recepcion;
                    local_item[recepcion]["sigma"] = sigma;
                    local_item[recepcion]["supervisor"] = supervisor;
                    local_item[recepcion]["respuesta"] = "";
                    local_item[recepcion]["hora_respuesta"] = "";
                    local_item[recepcion]["contra"] = "";
                    local_item[recepcion]["hora_contra"] = "";
                    local_item[recepcion]["descripcion"] = ui->label_description->text();
                    local_item[recepcion]["comentarios"] = ui->comentarios->toPlainText();
                    local_item[recepcion]["id"] = recepcion;

                   update_table(local_item);

                    //Restart the fields also to avoid overwritting
                    ui -> label_sigma -> setText("");
                    ui -> label_penalidad -> setText("");
                    ui -> label_ruta -> setText("");
                    ui -> label_movil -> setText("");
                    ui -> label_item -> setText("");
                    ui -> label_detalle -> setText("");
                    ui -> label_date -> setText("");
                    ui -> label_description ->setText("");
                    ui -> supervisor_1 ->setText("");
                    ui -> comentarios -> setPlainText("");

                    save("pendant");
                    actual_table = "general";
                    recall(local_item[recepcion]["sigma"],local_item[recepcion]["item"],recepcion);
        }
        else{
            QMessageBox::critical(this,"data","Campos incompletos");
        }
    }
    else {
        QMessageBox::critical(this,"data","Tipo de penalidad inválido");
    }
}

void Registro_penalidades::on_search_item_clicked()
{
    QString searcher = ui -> label_itemsearch ->text();

    //Verify the actual table
    QHash<QString,QHash<QString,QString>>filter_table;
    if(actual_table =="general"){
        filter_table = local_item;
    }
    else if (actual_table == "sigma"){
        filter_table = local_item;
        //filter_table = sigma_filter;
    }
    else if (actual_table == "item"){
        filter_table = local_item;
        //filter_table = item_filter;
    }

    //Verify if there is data in the searcher filter
    if(searcher!=""){

        //Start a Iterator to search into the local files
        QHashIterator<QString, QHash<QString, QString>> iterator(filter_table);
        QHash<QString,QHash<QString,QString>> local_counter;
        int aux_counter = 0;

        //Search with the filter in the local items
        while(iterator.hasNext()){
            auto key = iterator.next().key();

            if(filter_table[key]["item"] == searcher){
                local_counter[key]["item"] = filter_table[key]["item"];
                local_counter[key]["tipo"] = filter_table[key]["tipo"];
                local_counter[key]["ruta"] = filter_table[key]["ruta"];
                local_counter[key]["movil"] = filter_table[key]["movil"];
                local_counter[key]["detalle"] = filter_table[key]["detalle"];
                local_counter[key]["recepcion"] = filter_table[key]["recepcion"];
                local_counter[key]["sigma"] = filter_table[key]["sigma"];

                local_counter[key]["supervisor"] = filter_table[key]["supervisor"];
                local_counter[key]["respuesta"] = filter_table[key]["respuesta"];
                local_counter[key]["hora_respuesta"] = filter_table[key]["hora_respuesta"];
                local_counter[key]["contra"] = filter_table[key]["contra"];
                local_counter[key]["hora_contra"] = filter_table[key]["hora_contra"];
                local_counter[key]["descripcion"] = filter_table[key]["descripcion"];
                local_counter[key]["comentarios"] = filter_table[key]["comentarios"];
                local_counter[key]["id"] = filter_table[key]["id"];

                aux_counter++;
            }
        }
        if(aux_counter==0){
            QMessageBox::critical(this,"data","No existen registros con el item epecificado");
        }
        else{
            item_filter = local_counter;

            update_table(item_filter);

             actual_table = "item";
        }
    }
    else{
        QMessageBox::critical(this,"data","Colocar item para el filtro");
    }
}

void Registro_penalidades::on_search_sigma_clicked()
{
    QString searcher = ui -> label_sigmasearh ->text();

    //Verify the actual table
    QHash<QString,QHash<QString,QString>>filter_table;
    if(actual_table =="general"){
        filter_table = local_item;
    }
    else if (actual_table == "item"){
        filter_table = local_item;
        //filter_table = item_filter;
    }
    else if (actual_table == "sigma"){
        filter_table = local_item;
        //filter_table = sigma_filter;
    }

    //Verify if there is data in the searcher filter
    if(searcher!=""){

        //Start a Iterator to search into the local files
        QHashIterator<QString, QHash<QString, QString>> iterator(filter_table);
        QHash<QString,QHash<QString,QString>> local_counter;
        int aux_counter = 0;

        //Search with the filter in the local items
        while(iterator.hasNext()){
            auto key = iterator.next().key();

            if(filter_table[key]["sigma"] == searcher){
                local_counter[key]["item"] = filter_table[key]["item"];
                local_counter[key]["tipo"] = filter_table[key]["tipo"];
                local_counter[key]["ruta"] = filter_table[key]["ruta"];
                local_counter[key]["movil"] = filter_table[key]["movil"];
                local_counter[key]["detalle"] = filter_table[key]["detalle"];
                local_counter[key]["recepcion"] = filter_table[key]["recepcion"];
                local_counter[key]["sigma"] = filter_table[key]["sigma"];

                local_counter[key]["supervisor"] = filter_table[key]["supervisor"];
                local_counter[key]["respuesta"] = filter_table[key]["respuesta"];
                local_counter[key]["hora_respuesta"] = filter_table[key]["hora_respuesta"];
                local_counter[key]["contra"] = filter_table[key]["contra"];
                local_counter[key]["hora_contra"] = filter_table[key]["hora_contra"];
                local_counter[key]["descripcion"] = filter_table[key]["descripcion"];
                local_counter[key]["comentarios"] = filter_table[key]["comentarios"];
                local_counter[key]["id"] = filter_table[key]["id"];

                aux_counter++;
            }
        }
        if(aux_counter==0){
            QMessageBox::critical(this,"data","No existen registros con el item epecificado");
        }
        else{
            sigma_filter = local_counter;
            update_table(sigma_filter);
            actual_table = "sigma";
        }
    }
    else{
        QMessageBox::critical(this,"data","Colocar item para el filtro");
    }
}

void Registro_penalidades::on_button_quitar_clicked()
{
    ui->label_itemsearch -> setText("");
    ui->label_sigmasearh -> setText("");

    update_table(local_item);
    actual_table = "general";
}

void Registro_penalidades::on_table_gral_cellDoubleClicked(int row, int column)
{
    qDebug()<<row<<column;
    //This will be modified later for mod registers
}

void Registro_penalidades::on_button_respuesta_clicked()
{

    QString resp = ui -> text_respuesta -> toPlainText();
    QString time = ui-> label_date -> text();

    if (local_item[actual_id]["respuesta"]==""){

            if(resp !=""){

                //local_item[actual_id]["supervisor"] = super;
                local_item[actual_id]["respuesta"] = resp;
                local_item[actual_id]["hora_respuesta"] = time;

                ui -> table_gral -> setRowCount(0);

                update_table(local_item);

                //Restart the fields also to avoid overwritting
                ui -> label_sigma -> setText("");
                ui -> label_penalidad -> setText("");
                ui -> label_ruta -> setText("");
                ui -> label_movil -> setText("");
                ui -> label_item -> setText("");
                ui -> label_detalle -> setText("");
                ui -> label_date -> setText("");
                ui -> label_description ->setText("");

                ui -> comentarios -> setPlainText("");
                ui -> supervisor_1 -> setText("");

                ui -> text_respuesta -> setPlainText("");

                ui -> button_guardar ->setDisabled(false);
                ui -> button_respuesta->setDisabled(true);
                ui->butto_contrarespuesta->setDisabled(true);
                ui->button_update->setDisabled(true);
                save("pendant");
                actual_table = "general";
                actual_id = "";
            }
            else{
                QMessageBox::critical(this,"data","Rellenar los campos porfavor");
        }
    }
    else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Registro existente", "Desea sobreescribir?",QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::Yes){
                if(resp !=""){

                    //local_item[actual_id]["supervisor"] = super;
                    local_item[actual_id]["respuesta"] = resp;
                    local_item[actual_id]["hora_respuesta"] = time;

                    ui -> table_gral -> setRowCount(0);

                    update_table(local_item);

                    //Restart the fields also to avoid overwritting
                    ui -> label_sigma -> setText("");
                    ui -> label_penalidad -> setText("");
                    ui -> label_ruta -> setText("");
                    ui -> label_movil -> setText("");
                    ui -> label_item -> setText("");
                    ui -> label_detalle -> setText("");
                    ui -> label_date -> setText("");
                    ui -> label_description ->setText("");


                    ui ->supervisor_1->setText("");
                    ui -> comentarios -> setPlainText("");
                    ui -> supervisor_1 -> setText("");

                    ui -> text_respuesta -> setPlainText("");

                    ui -> button_guardar ->setDisabled(false);
                    ui -> button_respuesta->setDisabled(true);
                    ui->butto_contrarespuesta->setDisabled(true);
                    ui->button_update->setDisabled(true);
                    save("pendant");
                    actual_table = "general";
                    actual_id = "";
                }
            }
       }
}

void Registro_penalidades::on_butto_contrarespuesta_clicked()
{
    QString contra = ui -> text_contrarespuesta -> toPlainText();
    QString time = ui-> label_date -> text();

    if(local_item[actual_id]["contra"] == ""){
        if(contra!=""){

            local_item[actual_id]["contra"] = contra;
            local_item[actual_id]["hora_contra"] = time;

            ui -> table_gral -> setRowCount(0);

            update_table(local_item);

            //Restart the fields also to avoid overwritting
            ui -> label_sigma -> setText("");
            ui -> label_penalidad -> setText("");
            ui -> label_ruta -> setText("");
            ui -> label_movil -> setText("");
            ui -> label_item -> setText("");
            ui -> label_detalle -> setText("");
            ui -> label_date -> setText("");
            ui -> label_description ->setText("");

            ui -> comentarios -> setPlainText("");

            ui -> supervisor_1 -> setText("");

            ui -> text_respuesta -> setPlainText("");
            ui -> text_contrarespuesta -> setPlainText("");

            local_done.insert(actual_id, local_item[actual_id]);
            //local_item.remove(actual_id);
            eliminate_data<<actual_id;
            update_table(local_item);
            ui->button_guardar->setDisabled(false);
            //save("done");
            save("pendant");
            actual_table = "general";
            actual_id = "";
            ui -> button_respuesta->setDisabled(true);
            ui->button_update->setDisabled(true);
            ui->butto_contrarespuesta->setDisabled(true);
        }
        else{
            QMessageBox::critical(this,"data","Rellenar los campos porfavor");
        }
    }
    else{
        //Lock Button
    }
}


void Registro_penalidades::on_button_update_clicked()
{
    //This button sends the data to the local Hash and shows it in the table
    QString sigma = ui -> label_sigma -> text();
    QString tipo = ui -> label_penalidad -> text();
    QString ruta = ui -> label_ruta -> text();
    QString movil = ui -> label_movil -> text();
    QString item = ui -> label_item -> text();
    QString detalle = ui -> label_detalle -> toPlainText();
    QString recepcion = ui -> label_date -> text();
    QString supervisor = ui ->supervisor_1->text();

    QHashIterator<QString,QHash<QString,QString>>r_iter(db_rutas);
    QHashIterator<QString,QHash<QString,QString>>o_iter(db_overlords);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Actualizar", "Seguro desea actualizar estos valores?",QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
        if(tipo=="Infraccion"||tipo=="Deficiencia"){
            if(sigma!="" && tipo!=""  && item!=""){

                        while(r_iter.hasNext()){
                            auto v_key = r_iter.next().key();

                            if(db_rutas[v_key]["ruta"]==ruta){
                                local_item[actual_id]["ruta_id"] = v_key;
                                break;
                            }
                        }

                        while(o_iter.hasNext()){
                            auto o_key = o_iter.next().key();

                            if(o_key==supervisor){
                                local_item[actual_id]["supervisor_id"] = db_overlords[o_key]["id"];
                                break;
                            }
                        }

                        local_item[actual_id]["item"] = item;
                        local_item[actual_id]["tipo"] = tipo;
                        local_item[actual_id]["ruta"] = ruta;
                        local_item[actual_id]["movil"] = movil;
                        local_item[actual_id]["detalle"] = detalle;
                        local_item[actual_id]["sigma"] = sigma;
                         local_item[actual_id]["supervisor"] = supervisor;
                        local_item[actual_id]["descripcion"] = ui->label_description->text();
                        local_item[actual_id]["comentarios"]= ui->comentarios->toPlainText();

                        //Check this out
                        local_item[actual_id]["id"]= actual_id;

                       update_table(local_item);

                        //Restart the fields also to avoid overwritting
                        ui -> label_sigma -> setText("");
                        ui -> label_penalidad -> setText("");
                        ui -> label_ruta -> setText("");
                        ui -> label_movil -> setText("");
                        ui -> label_item -> setText("");
                        ui -> label_detalle -> setText("");
                        ui -> label_date -> setText("");
                        ui -> label_description ->setText("");
                        ui-> supervisor_1->setText("");
                        ui -> comentarios -> setPlainText("");

                        ui -> text_respuesta -> setPlainText("");
                        ui -> text_contrarespuesta -> setPlainText("");

                        save("pendant");
                        actual_table = "general";

                        ui->button_update->setDisabled(true);
                        ui->button_guardar->setDisabled(true);
                        ui->button_respuesta->setDisabled(true);
                        ui->butto_contrarespuesta->setDisabled(true);
            }
            else{
                QMessageBox::critical(this,"data","Campos incompletos");
            }
        }
        else {
            QMessageBox::critical(this,"data","Tipo de penalidad inválido");
        }
    }
}

void Registro_penalidades::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        ui -> label_sigma -> setText("");
        ui -> label_penalidad -> setText("");
        ui -> label_ruta -> setText("");
        ui -> label_movil -> setText("");
        ui -> label_item -> setText("");
        ui -> label_detalle -> setText("");
        ui -> label_date -> setText("");
        ui -> label_description ->setText("");

        ui -> supervisor_1 -> setText("");
        ui -> comentarios -> setPlainText("");

        ui -> text_respuesta -> setPlainText("");
        ui -> text_contrarespuesta -> setPlainText("");

        actual_table = "general";
        actual_id = "";
        ui->button_guardar->setDisabled(false);
        ui->button_respuesta->setDisabled(true);
        ui->butto_contrarespuesta->setDisabled(true);
    }
    if(event->key()==Qt::Key_Enter){
        //qDebug()<<"Hello world";
    }
}

void Registro_penalidades::read_routes(){
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


void Registro_penalidades::save(QString action){

    QJsonDocument documentoxd;
    QJsonObject datosxd;
    QJsonArray arrayDeDatos;
    QHash<QString, QHash<QString, QString>>saver;
    if(action == "pendant"){
        saver = local_item;
    }
    else{
        saver = local_done;
    }

    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();
        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;
        currnt.insert("id",item);
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

    QString filename= path+"/LPL_documents/"+action+".txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(documentoxd.toJson());
    file.close();

}

void Registro_penalidades::update_table(QHash<QString, QHash<QString,QString>>update){
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
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(update[current]["item"]));
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["tipo"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["sigma"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["ruta"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["movil"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["detalle"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["recepcion"]));
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["descripcion"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["supervisor"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(update[current]["respuesta"]));
        ui->table_gral->setItem(row_control, 10, new QTableWidgetItem(update[current]["hora_respuesta"]));
        ui->table_gral->setItem(row_control, 11, new QTableWidgetItem(update[current]["contra"]));
        ui->table_gral->setItem(row_control, 12, new QTableWidgetItem(update[current]["hora_contra"]));
        ui->table_gral->setItem(row_control, 13, new QTableWidgetItem(update[current]["comentarios"]));
        ui->table_gral->setItem(row_control, 14, new QTableWidgetItem(update[current]["id"]));

        if(update[current]["hora_contra"]!=""){
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

void Registro_penalidades::on_close_button_clicked()
{
    emit close_all();
    QHash<QString, QHash<QString, QString>>db = local_done;

    eliminate_data.removeDuplicates();

    foreach (QString item, eliminate_data) {
        local_done[item] = local_item[item];
        db[item] = local_item[item];
        local_item.remove(item);
    }
    save("pendant");
    saveJson(db);

}

bool Registro_penalidades::search_existing(QString search_for, QHash<QString,QHash<QString,QString>>container){

    QHashIterator<QString, QHash<QString, QString>>iter(container);
    int counter = 0;

    while(iter.hasNext()){

        auto current = iter.next().key();
        if(search_for == current){
            counter = 1;
            break;
        }
    }
    if(counter == 0){
        return false;
    }
    else{
        return true;
    }
}


void Registro_penalidades::on_clean_clicked()
{
    ui -> label_sigma -> setText("");
    ui -> label_penalidad -> setText("");
    ui -> label_ruta -> setText("");
    ui -> label_movil -> setText("");
    ui -> label_item -> setText("");
    ui -> label_detalle -> setText("");
    ui -> label_date -> setText("");
    ui -> label_description ->setText("");

    ui -> supervisor_1->setText("");
    ui -> comentarios -> setPlainText("");

    ui -> text_respuesta -> setPlainText("");
    ui -> text_contrarespuesta -> setPlainText("");

    actual_table = "general";
    actual_id = "";
    ui->button_guardar->setDisabled(false);
    ui->button_respuesta->setDisabled(true);
    ui->butto_contrarespuesta->setDisabled(true);
}

void Registro_penalidades::alarm_function(QString sigma, QString item, QString registro){

    QString respuesta = local_item[registro]["respuesta"];
    if(respuesta==""){
        QSound bells(":/resources/Recursos/alarm.wav");
        bells.play();
        QString time  = QDateTime::currentDateTime().toString("hh:mm");
        QMessageBox::warning(this,"Pasaron 45 minutos","Registro con sigma: "+sigma+", e item: "+item);
    }
}

void Registro_penalidades::on_anular_clicked()
{
    if(actual_id!=""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "Seguro desea eliminar este registro?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            local_item.remove(actual_id);
            local_done.remove(actual_id);
            save("done");
            save("pendant");
            update_table(local_item);
            on_clean_clicked();
        }
    }
}

void Registro_penalidades::recall(QString sigma, QString item,QString registro){
    QTimer::singleShot(2700000, [=](){
        alarm_function(sigma,item,registro);
    });
}

void Registro_penalidades::on_table_gral_cellClicked(int row, int column)
{
    //Get the position of the clicked cell
    QTableWidgetItem *itab = ui->table_gral->item(row,14);
    QString id = itab->text();
    qDebug()<<column;

    //charge all the variables to the new register
    ui -> label_sigma -> setText(local_item[id]["sigma"]);
    ui -> label_item -> setText(local_item[id]["item"]);
    ui -> label_ruta -> setText(local_item[id]["ruta"]);
    ui -> label_movil -> setText(local_item[id]["movil"]);
    ui -> label_detalle -> setText(local_item[id]["detalle"]);
    ui -> label_penalidad -> setText(local_item[id]["tipo"]);
    ui -> supervisor_1 -> setText(local_item[id]["supervisor"]);
    ui -> comentarios -> setPlainText(local_item[id]["comentarios"]);

    ui -> text_respuesta -> setPlainText(local_item[id]["respuesta"]);
    ui -> text_contrarespuesta -> setPlainText(local_item[id]["contra"]);

    //Set the description and sigma
    ui -> label_description -> setText(penalidades[local_item[id]["item"]]["Detalle"]);

    ui -> button_guardar -> setDisabled(true);
    ui -> button_update->setEnabled(true);
    actual_id = id;

    if(local_item[id]["respuesta"]!=""){

        ui->button_respuesta->setEnabled(true);
        ui->butto_contrarespuesta->setEnabled(true);
    }
    else{
        ui -> button_respuesta->setEnabled(true);
        ui->butto_contrarespuesta->setEnabled(false);
    }
}

/***********************************************************************
 **************************DATABASE READING**************************
*************************************************************************/

void Registro_penalidades::from_db_readVehicles(){

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

void Registro_penalidades::from_db_readStaff(){

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
void Registro_penalidades::from_db_readLink_1(){
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
void Registro_penalidades::from_db_readLink_2(){

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

void Registro_penalidades::from_db_readRoutes(){

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


void Registro_penalidades::from_db_readOverlords(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);

        foreach (QJsonValue entidad, okJson.object ().value ("supervisores").toArray ()) {

            QHash<QString, QString> current;
            //qDebug()<<entidad;
            current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ())); // ROUTES ID
            current.insert ("zona", entidad.toObject ().value ("zona").toString()); //Route name
            //current.insert ("supervisor", entidad.toObject ().value ("supervisor").toString()); //Route name

            db_overlords.insert(entidad.toObject ().value("zona").toString(), current);

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

        ui -> supervisor_1 -> setCompleter(super_completer);

        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/supervisor?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}


void Registro_penalidades::on_label_penalidad_editingFinished()
{
    QString type = ui -> label_penalidad-> text();
    QStringList penalties = {"Infraccion", "Deficiencia"};

    if(type != ""){
        if(!penalties.contains(type)){
            QMessageBox::critical (this, "Error", "Tipo de penalidad no válida");
            ui -> label_penalidad -> setText("");
        }
    }
}

void Registro_penalidades::on_label_ruta_editingFinished()
{
    QString type = ui -> label_ruta-> text();

    QHashIterator<QString, QHash<QString, QString>>iter(db_rutas);
    QString flag = "not";

    if(type != ""){
        while(iter.hasNext()){
            auto search = db_rutas[iter.next().key()]["ruta"];
            if(search==type){
                flag = "yes";
            }
        }
        if(flag == "not"){
            QMessageBox::critical (this, "Error", "Ruta inválida");
            ui -> label_ruta -> setText("");
        }
    }
}

void Registro_penalidades::on_label_movil_editingFinished()
{
    QString type = ui -> label_movil-> text();

    QHashIterator<QString, QHash<QString, QString>>iter(db_vehiculos);
    QString flag = "not";

    if(type != ""){
        while(iter.hasNext()){
            auto search = iter.next().key();
            if(search==type){
                flag = "yes";
            }
        }
        if(flag == "not"){
            QMessageBox::critical (this, "Error", "Ruta inválida");
            ui -> label_movil -> setText("");
        }
    }
}

void Registro_penalidades::on_supervisor_1_editingFinished()
{
    QString current_text = ui -> supervisor_1 -> text();

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
            ui -> supervisor_1 -> setText("");
        }
        else{
             ui -> supervisor_1 -> setText(current_text);
        }
    }
}

void Registro_penalidades::saveJson(QHash<QString,QHash<QString,QString>>saver){

    QJsonDocument document;
    QJsonArray main_array;

    //We need to create a virtual id duplicated container
    QStringList saved;

    QHashIterator<QString, QHash<QString, QString>>iter(saver);
    QHashIterator<QString, QHash<QString, QString>>r_iter(db_rutas);
    QHashIterator<QString, QHash<QString, QString>>o_iter(db_overlords);

    while(iter.hasNext()){
        auto main_key = iter.next().key();

        QJsonObject main_object;

        main_object.insert("item", saver[main_key]["item"]);
        main_object.insert("tipoDePenalidad", saver[main_key]["tipo"]);
        main_object.insert("detalle", saver[main_key]["detalle"]);
        main_object.insert("comentarios", saver[main_key]["comentarios"]);
        main_object.insert("sigma", saver[main_key]["sigma"]);

        main_object.insert("horaDeRecepcion",QDateTime::fromString(saver[main_key]["recepcion"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("horaDeRespuesta",QDateTime::fromString(saver[main_key]["hora_respuesta"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("horaDeContrarespuesta",QDateTime::fromString(saver[main_key]["hora_contra"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());

        main_object.insert("respuesta", saver[main_key]["respuesta"]);
        main_object.insert("contrarespuesta", saver[main_key]["contra"]);

        main_object.insert("ruta", saver[main_key]["ruta_id"]);
        main_object.insert("supervisor", saver[main_key]["supervisor_id"]);
        main_object.insert("movil", saver[main_key]["movil"]);
        main_object.insert("usuario", this -> user_name);

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
                    emit close();
                }
            }
        }
        else{
            local_done.clear();
            save("done");
            if(global_session == "yes"){
                  emit close();
            }
        }
        reply->deleteLater ();
    });

    QNetworkRequest request;
    request.setUrl (QUrl ("http://"+this -> url + "/penalties"));
    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");

    nam->post (request, document.toJson ());
}

void Registro_penalidades::save_data(){

    global_session = "no";
    QHash<QString, QHash<QString, QString>>db=local_done;

    eliminate_data.removeDuplicates();

    foreach (QString item, eliminate_data) {
        local_done[item] = local_item[item];
        db[item] = local_item[item];
        local_item.remove(item);
    }
    //save("done");
    save("pendant");
    saveJson(db);
}
