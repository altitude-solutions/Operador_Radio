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

    QIcon icon(":/images/img/LPL.png");
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
    connect(this,SIGNAL(enviar_nombre(QString)),registro_penalidades,SLOT(get_data(QString)));
    connect(this,SIGNAL(enviar_nombre(QString)),registro_datos,SLOT(get_data(QString)));
    connect(this,SIGNAL(enviar_nombre(QString)),registro_horarios,SLOT(get_data(QString)));

    //Close Session
     connect(registro_penalidades, &Registro_penalidades::close,this, &Operador_radio::closer);
     connect(registro_horarios, &Registro_horarios::logOut,this, &Operador_radio::closer);
     connect(registro_datos, &Registro_datos::logOut,this, &Operador_radio::closer);

}

Operador_radio::~Operador_radio()
{
    delete ui;
}

void Operador_radio::recibir_nombre(QString user){
    emit enviar_nombre(user);
}

void Operador_radio::closer(){

    delete  registro_horarios;
    delete registro_penalidades;
    delete registro_datos ;

    registro_horarios = new Registro_horarios(this);
    registro_penalidades = new Registro_penalidades(this);
    registro_datos = new Registro_datos(this);

    //Add tabs to the tab widget
    ui -> tabWidget -> addTab(registro_horarios, "Registro de Horarios");
    ui -> tabWidget -> addTab(registro_penalidades, "Registro de Penalidades");
    ui -> tabWidget -> addTab(registro_datos, "Registro de Datos");

    //Send the name to the next apps
    connect(this,SIGNAL(enviar_nombre(QString)),registro_penalidades,SLOT(get_data(QString)));
    connect(this,SIGNAL(enviar_nombre(QString)),registro_datos,SLOT(get_data(QString)));
    connect(this,SIGNAL(enviar_nombre(QString)),registro_horarios,SLOT(get_data(QString)));

    //Close Session
     connect(registro_penalidades, &Registro_penalidades::close,this, &Operador_radio::closer);
     connect(registro_horarios, &Registro_horarios::logOut,this, &Operador_radio::closer);
     connect(registro_datos, &Registro_datos::logOut,this, &Operador_radio::closer);

     this->close();
     emit logOut();
}

void Operador_radio::closeEvent(QCloseEvent *event){
    event -> ignore();
}
