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

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.25));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.4));

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

    //Define the data list
    QStringList lista_datos = { "Punto de Acopio",
                                              "Evacuar bolsas",
                                              "Bolseo",
                                              "can muerto",
                                              "Limpieza complementaria",
                                              "Barrido",
                                              "Mover obrera de barrido",
                                              "Poda",
                                              "Mantenimiento de contenedores",
                                              "Lavado",
                                              "Fregado"};


    foreach (QString itm, lista_datos){
            ui -> combo_dato -> addItem(itm);
     }

    ui -> combo_dato-> setEditable(true);
    ui -> combo_dato-> setCurrentIndex(-1);
    ui -> combo_dato-> setCurrentText("Seleccionar dato");

    //Set completer for the data label
//    QCompleter *data_completer = new QCompleter(lista_datos,this);

//    data_completer -> setCaseSensitivity(Qt::CaseInsensitive);
//    data_completer -> setCompletionMode(QCompleter::PopupCompletion);
//    data_completer -> setFilterMode(Qt::MatchContains);
//    ui -> label_dato -> setCompleter(data_completer);

    //Set the auxiliar normal by default
    auxiliar = "general";

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////WORKING TABLE SIZES//////////////////////////
   ////////////////////////////////////////////////////////////////////////////

    ui -> table_gral -> setColumnCount(19) ;
    ui->table_gral ->setColumnWidth(0,static_cast<int>(width/80));
    ui->table_gral ->setColumnWidth(1,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(2,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(3,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(4,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(5,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(6,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(7,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(8,static_cast<int>(width/30));
    ui->table_gral ->setColumnWidth(9,static_cast<int>(width/20));
    ui->table_gral ->setColumnWidth(10,static_cast<int>(width/10));
    for(int r=11; r<19; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/18));
    }

    //Setting the table headers
    QStringList headers = {"Id",
                           "Sigma",
                           "Dato",
                           "Zona",
                           "Calle",
                           "Detalle",
                           "Cantidad",
                           "Tipo",
                           "Codigo",
                           "Mantenimiento",
                           "Comentarios",
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
}

Registro_datos::~Registro_datos()
{
    delete ui;
}

void Registro_datos::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
    ui->label_date->setText(tiempo);
}

void Registro_datos::get_data(QString username){
    ui->label_user->setText(username);
}

void Registro_datos::enable(){

    QString option = ui-> combo_dato ->currentText();
    QString disabled = "background-color: white; "
                                  "border-style: outset; "
                                   "border-radius: 12px; "
                                   "font: 10pt \"MS Shell Dlg 2\"; "
                                   "color: #333333; "
                                   " min-width: 9em; "
                                   "padding: 3px;";
    QString enabled = "background-color: #EBEDED; "
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
    }
}

void Registro_datos::on_button_guardar_clicked()
{
    QString sigma = ui -> label_sigma -> text();
    QString dato = ui -> combo_dato ->currentText();
    QString zona = ui -> label_zona -> text();
    QString calle = ui -> label_calle -> text();
    QString detalle = ui -> label_detalle -> text();
    QString comentarios = ui -> text_comentarios -> toPlainText();
    QString cantidad = ui -> label_poda -> text();
    QString tipo = ui -> label_tipo -> text();
    QString codigo = ui -> label_codigo -> text();
    QString mantenimiento = ui -> label_mantenimiento -> text();
    QString time = QDateTime::currentDateTime().toString("dd.MM.yyyy")+" - "+QDateTime::currentDateTime().toString("hh:mm:ss");


    QString missing = "background-color: #EBEDED; "
                                 "border-style: outset; "
                                  "border-radius: 12px; "
                                  "font: 10pt \"MS Shell Dlg 2\"; "
                                  "color: #333333; "
                                  " min-width: 9em; "
                                  "padding: 3px;"
                                   "border-color:red;"
                                   "border-width:1px;";

    QString normal = "background-color: #EBEDED; "
                                 "border-style: outset; "
                                  "border-radius: 12px; "
                                  "font: 10pt \"MS Shell Dlg 2\"; "
                                  "color: #333333; "
                                  " min-width: 9em; "
                                  "padding: 3px;";

    ui -> label_sigma -> setStyleSheet(normal);
    ui -> combo_dato -> setStyleSheet(normal);

    if(lock!=true){
        if(sigma !="" && dato!="Seleccionar dato"){
            if(auxiliar == "general"){
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;
                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["numero_dato"]=QString::number(counter);
                    temporal[time]["hora"]=time;

                    temporal[time]["cantidad"]="-";
                    temporal[time]["tipo"]="-";
                    temporal[time]["codigo"]="-";
                    temporal[time]["mantenimiento"]="-";

                    update_table(temporal);
                    counter++;
                    save("pendant");
                    save_counter(counter);

                    restart();
            }
            else if (auxiliar == "poda"){

                if(cantidad!=""){
                    ui -> label_poda -> setStyleSheet(normal);
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;
                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["numero_dato"]=QString::number(counter);
                    temporal[time]["hora"]=time;

                    temporal[time]["cantidad"]=cantidad;
                    temporal[time]["tipo"]="-";
                    temporal[time]["codigo"]="-";
                    temporal[time]["mantenimiento"]="-";

                    update_table(temporal);
                    counter++;
                    save("pendant");
                    save_counter(counter);

                    restart();
                }
                else{
                    QMessageBox::critical(this,"data","Rellenar los campos obligatorios porfavor");
                    if(cantidad == ""){
                         ui -> label_poda -> setStyleSheet(missing);
                    }
                    else{
                        ui -> label_poda -> setStyleSheet(normal);
                    }
                }
            }
            else if (auxiliar == "mantenimiento"){
                if(tipo!=""&&codigo!=""&&mantenimiento!=""){
                    ui -> label_mantenimiento -> setStyleSheet(normal);
                    ui -> label_codigo -> setStyleSheet(normal);
                    ui -> label_tipo -> setStyleSheet(normal);
                    temporal[time]["sigma"]=sigma;
                    temporal[time]["dato"]=dato;
                    temporal[time]["zona"]=zona;
                    temporal[time]["calle"]=calle;
                    temporal[time]["detalle"]=detalle;
                    temporal[time]["comentarios"]=comentarios;
                    temporal[time]["numero_dato"]=QString::number(counter);
                    temporal[time]["hora"]=time;

                    temporal[time]["cantidad"]="-";
                    temporal[time]["tipo"] = tipo;
                    temporal[time]["codigo"] = codigo;
                    temporal[time]["mantenimiento"] = mantenimiento;

                    update_table(temporal);
                    counter++;
                    save("pendant");
                    save_counter(counter);

                    restart();
                }
                else{
                        QMessageBox::critical(this,"data","Rellenar los campos obligatorios porfavor");
                        if (tipo==""){
                            ui -> label_tipo -> setStyleSheet(missing);
                        }
                        else{
                            ui -> label_tipo -> setStyleSheet(normal);
                        }
                        if(codigo==""){
                            ui -> label_codigo -> setStyleSheet(missing);
                        }
                        else{
                            ui -> label_codigo -> setStyleSheet(normal);
                        }
                        if(mantenimiento==""){
                            ui -> label_mantenimiento -> setStyleSheet(missing);
                        }
                        else{
                            ui -> label_mantenimiento -> setStyleSheet(normal);
                        }
                 }
            }
        }
        else{
             QMessageBox::critical(this,"data","Rellenar los campos obligatorios porfavor");

             if (sigma==""){
                  ui -> label_sigma -> setStyleSheet(missing);
             }
             else{
                 ui -> label_sigma -> setStyleSheet(normal);
             }

             if(dato=="Seleccionar dato"){
                 ui -> combo_dato -> setStyleSheet(missing);
             }
             else{
                 ui -> combo_dato -> setStyleSheet(normal);
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
        current.insert("zona", objetoxd.toObject().value("zona").toString());
        current.insert("calle",objetoxd.toObject().value("calle").toString());
        current.insert("detalle",objetoxd.toObject().value("detalle").toString());
        current.insert("cantidad",objetoxd.toObject().value("cantidad").toString());
        current.insert("tipo",objetoxd.toObject().value("tipo").toString());
        current.insert("codigo",objetoxd.toObject().value("codigo").toString());

        current.insert("mantenimiento",objetoxd.toObject().value("mantenimiento").toString());
        current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());

        current.insert("comunicación",objetoxd.toObject().value("comunicación").toString());
        current.insert("ejecucion",objetoxd.toObject().value("ejecucion").toString());
        current.insert("verificacion",objetoxd.toObject().value("verificacion").toString());
        current.insert("conciliacion",objetoxd.toObject().value("conciliacion").toString());

        current.insert("hora_com",objetoxd.toObject().value("hora_com").toString());
        current.insert("hora_ejec",objetoxd.toObject().value("hora_ejec").toString());
        current.insert("hora_ver",objetoxd.toObject().value("hora_ver").toString());
        current.insert("hora_conc",objetoxd.toObject().value("hora_conc").toString());

        current.insert("numero_dato",objetoxd.toObject().value("numero_dato").toString());

        done.insert(objetoxd.toObject().value("hora").toString(),current);
    }
}

void Registro_datos::read_temporal(){

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
        QHash<QString,QString> current;
        current.insert("sigma", objetoxd.toObject().value("sigma").toString());
        current.insert("dato", objetoxd.toObject().value("dato").toString());
        current.insert("zona", objetoxd.toObject().value("zona").toString());
        current.insert("calle",objetoxd.toObject().value("calle").toString());
        current.insert("detalle",objetoxd.toObject().value("detalle").toString());
        current.insert("cantidad",objetoxd.toObject().value("cantidad").toString());
        current.insert("tipo",objetoxd.toObject().value("tipo").toString());
        current.insert("codigo",objetoxd.toObject().value("codigo").toString());

        current.insert("mantenimiento",objetoxd.toObject().value("mantenimiento").toString());
        current.insert("comentarios",objetoxd.toObject().value("comentarios").toString());
        current.insert("comunicación",objetoxd.toObject().value("comunicación").toString());

        current.insert("ejecucion",objetoxd.toObject().value("ejecucion").toString());
        current.insert("verificacion",objetoxd.toObject().value("verificacion").toString());
        current.insert("conciliacion",objetoxd.toObject().value("conciliacion").toString());

        current.insert("hora_com",objetoxd.toObject().value("hora_com").toString());
        current.insert("hora_ejec",objetoxd.toObject().value("hora_ejec").toString());
        current.insert("hora_ver",objetoxd.toObject().value("hora_ver").toString());
        current.insert("hora_conc",objetoxd.toObject().value("hora_conc").toString());

        current.insert("numero_dato",objetoxd.toObject().value("numero_dato").toString());

        temporal.insert(objetoxd.toObject().value("hora").toString(),current);
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
    if (spplitTime[0] >= 6 && split_data[0] < 6){
        counter = 0;
    }
    else{
        if(split_data[0].toInt()<6000){
            counter = split_data[0].toInt();
        }
        else{
            counter = 0;
        }
    }

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

        //Writing the current row
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(update[current]["numero_dato"]));
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["sigma"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["dato"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["zona"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["calle"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["detalle"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["cantidad"]));
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["tipo"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["codigo"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(update[current]["mantenimiento"]));
        ui->table_gral->setItem(row_control, 10, new QTableWidgetItem(update[current]["comentarios"]));
        ui->table_gral->setItem(row_control, 11, new QTableWidgetItem(update[current]["comunicación"]));
        ui->table_gral->setItem(row_control, 12, new QTableWidgetItem(update[current]["hora_com"]));
        ui->table_gral->setItem(row_control, 13, new QTableWidgetItem(update[current]["ejecucion"]));
        ui->table_gral->setItem(row_control, 14, new QTableWidgetItem(update[current]["hora_ejec"]));
        ui->table_gral->setItem(row_control, 15, new QTableWidgetItem(update[current]["verificacion"]));
        ui->table_gral->setItem(row_control, 16, new QTableWidgetItem(update[current]["hora_ver"]));
        ui->table_gral->setItem(row_control, 17, new QTableWidgetItem(update[current]["conciliacion"]));
        ui->table_gral->setItem(row_control, 18, new QTableWidgetItem(update[current]["hora_conc"]));

    }
    ui->table_gral->setSortingEnabled(true);
    ui->table_gral->sortByColumn(0,Qt::AscendingOrder);
}

void Registro_datos::on_search_item_clicked()
{
    QString search = ui -> label_search -> text();
    QString random = search_dato(search);
    QString actual_value = ui->combo_dato->currentText();

    int counter = 0;
    //Search for the register in the table
    QHashIterator<QString, QHash<QString, QString>>iter(temporal);

    while(iter.hasNext()){
        auto current = iter.next().key();
        if (current==random){
            counter = 1;
            break;
        }
    }

    if(counter == 1){
          //ui -> label_dato -> setText(random);
          ui -> label_sigma -> setText(temporal[random]["sigma"]);
          ui -> combo_dato ->setCurrentText(temporal[random]["dato"]);
          ui -> label_zona -> setText(temporal[random]["zona"]);
          ui -> label_calle -> setText(temporal[random]["calle"]);
          ui -> label_detalle -> setText(temporal[random]["detalle"]);
          ui -> text_comentarios -> setPlainText(temporal[random]["comentarios"]);
          ui -> label_poda -> setText(temporal[random]["cantidad"]);
          ui -> label_tipo -> setText(temporal[random]["tipo"]);
          ui -> label_codigo -> setText(temporal[random]["codigo"]);
          ui -> label_mantenimiento -> setText(temporal[random]["mantenimiento"]);

          ui -> comunicacion -> setText(temporal[random]["comunicación"]);
          ui -> ejecucion -> setText(temporal[random]["ejecucion"]);
          ui -> verificacion -> setText(temporal[random]["verificacion"]);
          ui -> conciliacion -> setText(temporal[random]["conciliacion"]);

          lock = true;
          lock_b = false;

          if(actual_value=="Mantenimiento de contenedores"){
              auxiliar ="mantenimiento";
          }
          else if(actual_value == "Poda"){
              auxiliar = "poda";
          }
          else{
              auxiliar = "general";
          }

    }
    else{
         QMessageBox::critical(this,"data","Dato no registrado");
    }
}

//TODO RESET ALLAFTER SAVING SOMETHING
void Registro_datos::on_button_respuesta_clicked()
{
    QString current = ui -> comunicacion -> text();
    QString search = ui -> label_search -> text();
    QString random = search_dato(search);
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    if (lock_b!=true){
        if(current!=""){
            temporal[random]["comunicación"]=current;
            temporal[random]["hora_com"]=time;
            update_table(temporal);
            save("pendant");
            restart();
            lock = false;
            lock_b = true;

        }
        else{
            QMessageBox::critical(this,"data","Ingresar campos porfavor");
        }
    }
}

void Registro_datos::on_button_respuesta_2_clicked()
{
    QString current = ui -> ejecucion -> text();
    QString search = ui -> label_search -> text();
    QString random = search_dato(search);
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    if(lock_b!=true){
        if(current!=""){
            if(auxiliar!="general"){
                temporal[random]["ejecucion"]=current;
                temporal[random]["hora_ejec"]=time;
                update_table(temporal);
                save("pendant");
                restart();
                lock = false;
                lock_b = true;
            }
            else{
                temporal[random]["ejecucion"]=current;
                temporal[random]["hora_ejec"]=time;
                done[random] = temporal[random];

                update_table(temporal);
                eliminate_list<<random;

                save("pendant");
                save("done");
                restart();
                lock = false;
                lock_b = true;
            }
        }
        else{
            QMessageBox::critical(this,"data","Ingresar campos porfavor");
        }
    }
}

void Registro_datos::on_button_respuesta_4_clicked()
{
    QString current = ui -> verificacion -> text();
    QString search = ui -> label_search -> text();
    QString random = search_dato(search);
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    if(lock_b!=true){
        if(current!=""){
            if(auxiliar!="general"){
                temporal[random]["verificacion"]=current;
                temporal[random]["hora_ver"]=time;
                update_table(temporal);

                save("pendant");
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
}

void Registro_datos::on_button_respuesta_3_clicked()
{
    QString current = ui -> conciliacion -> text();
    QString search = ui -> label_search -> text();
    QString random = search_dato(search);
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    if(lock_b!=true){
        if(current!=""){
            if(auxiliar!="general"){
                temporal[random]["conciliacion"]=current;
                temporal[random]["hora_conc"]=time;
                update_table(temporal);
                eliminate_list<<random;

                save("pendant");
                save("done");
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
}

void Registro_datos::on_close_button_clicked()
{
    foreach (QString val, eliminate_list) {
        temporal.remove(val);
    }
    save("pendant");
    emit logOut();
}

QString Registro_datos::search_dato(QString item){
    //Start a QIterator for the local movil where we are searching the movil
    QHashIterator<QString, QHash<QString, QString>>iter(temporal);
    QString key_search;

    while(iter.hasNext()){

        auto current = iter.next().key();
        if(temporal[current]["numero_dato"]==item){
            key_search = current;
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
    ui -> label_sigma -> setText("");
    //ui -> label_dato -> setText("");
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

    ui -> combo_dato-> setEditable(true);
    ui -> combo_dato-> setCurrentIndex(-1);
    ui -> combo_dato-> setCurrentText("Seleccionar dato");
}
