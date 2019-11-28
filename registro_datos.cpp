#include "registro_datos.h"
#include "ui_registro_datos.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>
#include <QCompleter>

Registro_datos::Registro_datos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_datos)
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
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.25));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.4));


    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

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

    //Set completer for the data label
    QCompleter *data_completer = new QCompleter(lista_datos,this);

    data_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    data_completer -> setCompletionMode(QCompleter::PopupCompletion);
    ui -> label_dato -> setCompleter(data_completer);

    //Set the auxiliar normal by default
    auxiliar = "general";

    //Set the table Size
    ui -> table_gral -> setColumnCount(15) ;
    ui->table_gral ->setColumnWidth(0,static_cast<int>(width/40));
    for(int r=1; r<15; r++){
        ui->table_gral ->setColumnWidth(r,static_cast<int>(width/15.2));
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
                           "Ejecución",
                           "Verificación",
                           "Conciliación"};

    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////CONNECTIONS////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    //connect between the item and the description
    connect(ui->label_dato,SIGNAL(editingFinished()),this,SLOT(enable()));
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

    QString option = ui-> label_dato -> text();
    QString disabled = "background-color: white; "
                                  "border-style: outset; "
                                   "border-radius: 12px; "
                                   "font: 10pt \"MS Shell Dlg 2\"; "
                                   "color: #333333; "
                                   " min-width: 9em; "
                                   "padding: 3px;";
    QString enabled = "background-color: #EBEDED; "
                                 "border-style: outset; "
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

        //Clean Poda
        ui -> label_poda -> setText("");

        auxiliar = "Mantenimiento";
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
    if(auxiliar == "general"){

    }
    else if (auxiliar == "poda"){

    }
    else if (auxiliar == "mante"){

    }
}
