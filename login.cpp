#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QDesktopWidget>
#include <QDebug>
#include <QScreen>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    //operador_radio = new Operador_radio(this);

    operador_radio.hide();

    //Get screen Size
   const auto screens = qApp->screens();

   int width = screens[0]->geometry().width();
   int height = screens[0]->geometry().height();

    //set widget size dynamic, aspect ratio 16:9
    double size_w = (width*400)/1920;
    double size_h = (height*280)/1080;
    QSize size (static_cast<int>(size_w), static_cast<int>(size_h));
    this->setFixedSize(size);

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*160)/1920;
    double pix_h = (height*160)/1080;
    QPixmap pix(":/images/img/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //connect event for labels to button
    connect(ui->user, SIGNAL(returnPressed()),ui->login_button,SLOT(click()));
    connect(ui->password, SIGNAL(returnPressed()),ui->login_button,SLOT(click()));

    //Send the name to the next window
    connect(this,SIGNAL(authDataRetrieved(QString, QString, QString)), &operador_radio, SLOT(recibir_nombre(QString, QString, QString)));
    //connect(this,SIGNAL(send_name(QString)),&operador_radio,SLOT(recibir_nombre(QString)));

    //Close the session
    connect(&operador_radio, &Operador_radio::logOut, this, &Login::cerrar);
}

Login::~Login()
{
    delete ui;
}

//Function to call the second MainWindow Widget with a button click
void Login::on_login_button_clicked()
{
    /*************************************************************************
                                             Network Connection
    **************************************************************************/
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    //Lambda function
    connect(nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
            QByteArray resBin = reply->readAll ();
            if (reply->error ()) {
                QJsonDocument errorJson = QJsonDocument::fromJson( resBin );

                //error catch
                if (errorJson.object ().value ("err").toObject ().contains ("message")) {
                    QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
                }
                else {
                    QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
                }
                ui -> login_button -> setEnabled (true);
                return;
            }

            QJsonDocument response = QJsonDocument::fromJson ( resBin );
            QStringList permissions;

            foreach (QJsonValue perm, response.object ().value ("user").toObject ().value ("permisos").toArray ()) {
                permissions << perm.toString ();
            }

            //qDebug () << "Permisos" << permissions;

            ui -> login_button->setEnabled (true);
            emit authDataRetrieved (response.object().value("user").toObject().value("nombreUsuario").toString(), QString::fromLatin1 ( response.object().value("user").toObject().value("nombreReal").toString().toLatin1() ), response.object().value("token").toString());

            operador_radio.show ();
            this->hide ();

        });

    QNetworkRequest req;

    //TODO --> Change to config file
    req.setUrl (QUrl ("http://192.168.0.5:3000/login"));
    req.setRawHeader ("Content-Type", "application/json");

    QJsonDocument body;
    QJsonObject bodyContent;

    bodyContent.insert ("nombreUsuario", ui -> user ->text ());
    bodyContent.insert ("contra", ui -> password->text ());
    body.setObject (bodyContent);

    nam->post(req, body.toJson());

    ui -> login_button -> setDisabled(true);

//    QString name = ui -> user ->text();
//    emit send_name(name);

}

void Login::cerrar(){

    operador_radio.hide();

    ui ->user -> setText("");
    ui -> password -> setText("");

    this->show();

}
