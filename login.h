#ifndef LOGIN_H
#define LOGIN_H

#include "mainwindow.h"

#include <QSettings>
#include <QWidget>

namespace Ui {
class Login;
}
extern QSettings settings;
extern bool logined;
extern bool authorization;
extern QString host;
extern QString localHost;
extern QString httpPost(QString link,QByteArray postData);
extern QString HttpGet(QString link);
extern QString getHostMacAddress();
extern MainWindow *fc;
class Login : public QWidget
{
    Q_OBJECT

public:

    void login(bool autoLogin);
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_commandLinkButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();


private:
    Ui::Login *ui;
};

#endif // LOGIN_H
