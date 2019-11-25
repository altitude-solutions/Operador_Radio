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

    //connect between the lines and the save button
    connect(ui->label_sigma, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_penalidad, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_movil, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_ruta, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));
    connect(ui->label_detalle, SIGNAL(returnPressed()),ui->button_guardar, SLOT(click()));

    //connect the filters
    connect(ui->label_sigmasearh, SIGNAL(returnPressed()),ui->search_sigma,SLOT(click()));
    connect(ui->label_itemsearch, SIGNAL(returnPressed()),ui->search_item,SLOT(click()));

    //Set the acutal table by default
    actual_table = "general";

    //set the button locker off
    lock = false;

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
        current.insert("movil",object.toObject().value("movil").toString());
        current.insert("detalle",object.toObject().value("detalle").toString());
        current.insert("recepcion",object.toObject().value("recepcion").toString());

        current.insert("sigma",object.toObject().value("sigma").toString());
        current.insert("supervisor",object.toObject().value("supervisor").toString());
        current.insert("respuesta",object.toObject().value("respuesta").toString());

        current.insert("hora_respuesta",object.toObject().value("hora_respuesta").toString());
        current.insert("contra",object.toObject().value("contra").toString());
        current.insert("hora_contra",object.toObject().value("hora_contra").toString());

        local_item.insert(object.toObject().value("recepcion").toString(),current);

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

        local_done.insert(object.toObject().value("recepcion").toString(),current);
    }
}

Registro_penalidades::~Registro_penalidades()
{
    delete ui;
}

void Registro_penalidades::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd.MM.yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");
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
        ui->label_item->setText("");
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

    //Verify the lock
    if (lock == false){
        //Verify if the fields are empty
        if(sigma!="" && tipo!="" && ruta!="" && movil!="" && item!=""){

            local_item[recepcion]["item"] = item;
            local_item[recepcion]["tipo"] = tipo;
            local_item[recepcion]["ruta"] = ruta;
            local_item[recepcion]["movil"] = movil;
            local_item[recepcion]["detalle"] = detalle;
            local_item[recepcion]["recepcion"] = recepcion;
            local_item[recepcion]["sigma"] = sigma;
            local_item[recepcion]["supervisor"] = "";
            local_item[recepcion]["respuesta"] = "";
            local_item[recepcion]["hora_respuesta"] = "";
            local_item[recepcion]["contra"] = "";
            local_item[recepcion]["hora_contra"] = "";

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

            save("pendant");
            actual_table = "general";
        }
        else{
            QMessageBox::critical(this,"data","Campos incompletos");
        }
    }
    else{
       //  QMessageBox::critical(this,"data","");
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
        filter_table = sigma_filter;
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
        filter_table = item_filter;
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
    //Get the position of the clicked cell
    QTableWidgetItem *itab = ui->table_gral->item(row,6);
    QString id = itab->text();

    //charge all the variables to the new register
    ui -> label_sigma -> setText(local_item[id]["sigma"]);
    ui -> label_item -> setText(local_item[id]["item"]);
    ui -> label_ruta -> setText(local_item[id]["ruta"]);
    ui -> label_movil -> setText(local_item[id]["movil"]);
    ui -> label_detalle -> setText(local_item[id]["detalle"]);
    ui -> label_penalidad -> setText(local_item[id]["tipo"]);

    ui -> label_supervisor -> setText(local_item[id]["supervisor"]);
    ui -> text_respuesta -> setPlainText(local_item[id]["respuesta"]);
    ui -> text_contrarespuesta -> setPlainText(local_item[id]["contra"]);

    //Set the description and sigma
    ui -> label_description -> setText(penalidades[local_item[id]["item"]]["Detalle"]);
    ui -> sigma_2 -> setText(local_item[id]["sigma"]);

    lock = true;
    actual_id = id;
}

void Registro_penalidades::on_button_respuesta_clicked()
{
    QString super = ui -> label_supervisor -> text();
    QString resp = ui -> text_respuesta -> toPlainText();
    QString time = ui-> label_date -> text();
    if(local_item[actual_id]["supervisor"] == ""){
        if(super!="" && resp !=""){

            local_item[actual_id]["supervisor"] = super;
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
            ui -> sigma_2 -> setText("");

            ui -> label_supervisor -> setText("");
            ui -> text_respuesta -> setPlainText("");

            lock = false;
            save("pendant");
            actual_table = "general";
            actual_id = "";
        }
        else{
            QMessageBox::critical(this,"data","Rellenar los campos porfavor");
        }
    }
    else{
        //Lock Button
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
            ui -> sigma_2 -> setText("");

            ui -> label_supervisor -> setText("");
            ui -> text_respuesta -> setPlainText("");
            ui -> text_contrarespuesta -> setPlainText("");

            local_done.insert(actual_id, local_item[actual_id]);
            local_item.remove(actual_id);
            update_table(local_item);
            lock = false;
            save("done");
            save("pendant");
            actual_table = "general";
            actual_id = "";
        }
        else{
            QMessageBox::critical(this,"data","Rellenar los campos porfavor");
        }
    }
    else{
        //Lock Button
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
        ui -> sigma_2 -> setText("");

        ui -> label_supervisor -> setText("");
        ui -> text_respuesta -> setPlainText("");
        ui -> text_contrarespuesta -> setPlainText("");

        actual_table = "general";
        actual_id = "";
        lock  = false;
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
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["supervisor"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["respuesta"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(update[current]["hora_respuesta"]));
        ui->table_gral->setItem(row_control, 10, new QTableWidgetItem(update[current]["contra"]));
        ui->table_gral->setItem(row_control, 11, new QTableWidgetItem(update[current]["hora_contra"]));
    }
}

void Registro_penalidades::on_close_button_clicked()
{
    emit close();
}
