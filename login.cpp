#include "login.h"
#include "ui_login.h"

#include <QDesktopServices>
#include <QMessageBox>
extern int arg;
void Login::login(bool autoLogin)
{
    QString email = settings.value("email").toString();
    QString password = settings.value("password").toString();
    if(!email.isEmpty()&&!password.isEmpty()){

        QByteArray qba;
        qba.append("email="+email.toUtf8());
        qba.append("&password="+password.toUtf8());
        QString res=httpPost("Auth/ntclogin",qba);

        if(res.contains("ok")){
            logined=true;
            HttpGet("Auth/setMac?email="+email+"&password="+password+"&mac="+getHostMacAddress());
            QString res=HttpGet("Auth/getNtcAuth?email="+email+"&password="+password);
            bool ok;
            int auth=res.toInt(&ok);
            if(ok&&auth==1){
                authorization=true;
            }else{
                authorization=false;
            }
            this->hide();
            if(arg!=2){
                fc->showNormal();
            }



        }else{
            logined=false;
            authorization=false;

             QMessageBox::critical(this, tr("错误"), "用户名或密码错误或网络异常", QMessageBox::Ok);

        }

    }
}

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("登录");
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    login(true);

}

Login::~Login()
{
    delete ui;
}

void Login::on_commandLinkButton_clicked()
{
    QUrl url("https://qm.qq.com/cgi-bin/qm/qr?k=Bvs2uQxc0rCDzn0Wklaw76wfUdxO1dV2&jump_from=webapi&authKey=vTkp0Nojj6owKfLwmPCRWSc2qdkUzP5FLmhU9SIdkSoR8pp4i/PinyGmh5rxfO8m");
    QDesktopServices::openUrl(url);
}


void Login::on_pushButton_clicked()
{
    QUrl url(host+"Auth/register");
    QDesktopServices::openUrl(url);
}


void Login::on_pushButton_3_clicked()
{
    QUrl url(host+"Auth/find");
    QDesktopServices::openUrl(url);
}


void Login::on_pushButton_2_clicked()
{
    settings.setValue("email",ui->lineEdit->text());
    settings.setValue("password",ui->lineEdit_2->text());
    login(false);
}

