#include "operador_radio.h"
#include "ui_operador_radio.h"
#include <QDesktopWidget>
#include <QScreen>
#include <QtDebug>
#include <QCloseEvent>

Operador_radio::Operador_radio(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Operador_radio)
{
    ui->setupUi(this);

    QIcon icon(":/images/LPL.ico");
    this -> setWindowIcon(icon);

    //Define the specific widgets for each tab
    registro_horarios = new Registro_horarios(this);
    registro_penalidades = new Registro_penalidades(this);
    registro_datos = new Registro_datos(this);

    this->setWindowTitle("Operador de radio");

     //Get screen Size
    const auto screens = qApp->screens();

    int width = screens[0]->geometry().width();
    int height = screens[0]->geometry().height();

    //Set Widget not resizable
    this->setMinimumWidth(width);
    this->setMinimumHeight(height);

    //set widget size maximized
    this->setWindowState(Qt::WindowMaximized);

    //set tab widget fixed Size
    QSize size = qApp->screens()[0]->size();
    ui -> tabWidget ->setFixedSize(size);

    //Add tabs to the tab widget
    ui -> tabWidget -> addTab(registro_horarios, "Registro de Horarios");
    ui -> tabWidget -> addTab(registro_penalidades, "Registro de Penalidades");
    ui -> tabWidget -> addTab(registro_datos, "Registro de Datos");

    //Send the name to the next apps
    connect(this,SIGNAL(enviar_informacion(QString, QString, QString)),registro_penalidades,SLOT(get_data(QString, QString, QString)));
    connect(this,SIGNAL(enviar_informacion(QString, QString, QString)),registro_datos,SLOT(get_data(QString, QString, QString)));
    connect(this,SIGNAL(enviar_informacion(QString, QString, QString)),registro_horarios,SLOT(get_data(QString, QString, QString)));

    connect(this,SIGNAL(send_url(QString)),registro_penalidades,SLOT(get_url(QString)));
    connect(this,SIGNAL(send_url(QString)),registro_datos,SLOT(get_url(QString)));
    connect(this,SIGNAL(send_url(QString)),registro_horarios,SLOT(get_url(QString)));

    connect(registro_horarios, SIGNAL(close_all()), registro_penalidades, SLOT(save_data()));
    connect(registro_horarios, SIGNAL(close_all()), registro_datos, SLOT(save_data()));

    connect(registro_penalidades, SIGNAL(close_all()), registro_horarios, SLOT(save_data()));
    connect(registro_penalidades, SIGNAL(close_all()), registro_datos, SLOT(save_data()));

    connect(registro_datos, SIGNAL(close_all()), registro_penalidades, SLOT(save_data()));
    connect(registro_datos, SIGNAL(close_all()), registro_horarios, SLOT(save_data()));


    //Close Session
     connect(registro_penalidades, &Registro_penalidades::close,this, &Operador_radio::closer);
     connect(registro_horarios, &Registro_horarios::logOut,this, &Operador_radio::closer);
     connect(registro_datos, &Registro_datos::logOut,this, &Operador_radio::closer);

}

Operador_radio::~Operador_radio()
{
    delete ui;
}

void Operador_radio::recibir_nombre(QString user, QString real, QString token){
    emit enviar_informacion(real, user, token);
}

void Operador_radio::receive_url(QString url){
    this->url = url;
    emit send_url(this->url);
}

void Operador_radio::closer(){

     this->close();
     emit logOut();
}

void Operador_radio::closeEvent(QCloseEvent *event){
    event -> ignore();
}
