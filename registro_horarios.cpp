#include "registro_horarios.h"
#include "ui_registro_horarios.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>

Registro_horarios::Registro_horarios(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro_horarios)
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

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.13));   

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);
}

Registro_horarios::~Registro_horarios()
{
    delete ui;
}

void Registro_horarios::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm");
    ui->label_date->setText(tiempo);
}

void Registro_horarios::get_data(QString username){
    ui->label_user->setText(username);
}
