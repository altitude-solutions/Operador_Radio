#include "registro_penalidades.h"
#include "ui_registro_penalidades.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>

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
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.4));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.05));
    ui -> frame_4 -> setFixedHeight(static_cast<int>(height*0.3));

    //Set Search icons
    QPixmap pix_b1(":/images/img/search.png");
    QIcon ButtonIcon(pix_b1);
    ui->search_item->setIcon(ButtonIcon);
    ui->search_item->setIconSize(QSize(20,20));
    ui->search_movil->setIcon(ButtonIcon);
    ui->search_movil ->setIconSize(QSize(20,20));
    ui->search_sigma->setIcon(ButtonIcon);
    ui->search_sigma->setIconSize(QSize(20,20));

    //Setting the table headers
    QStringList headers = {"Item","tipo de penalidad","Sigma","Ruta","Movil","Detalle","Hora recepción","Supervisor","Respuesta","Hora Respuesta","Contra Respuesta", "Hora contrarespuesta"};
    ui -> table_gral -> setHorizontalHeaderLabels(headers);

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);
}

Registro_penalidades::~Registro_penalidades()
{
    delete ui;
}

void Registro_penalidades::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
    ui->label_date->setText(tiempo);
}

void Registro_penalidades::get_data(QString username){
    ui->label_user->setText(username);
}
