#include "mainwindow.h"
#include "note.h"
#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QTimer>
#include <QUrlQuery>

Note *note;
MainWindow *fc;
QString text;
QNetworkAccessManager *networkManager;
QSettings settings("NoteToCard","Settings");
QString host="https://clipnote.cn/";
QString localHost="http://127.0.0.1:5437/";
QString httpGet(QString link);

void clickSelf(){
    fc->clickSelf();
}
void updateTree(){
    if(!settings.value("note").toString().isEmpty()){
        fc->setTreeView(fc->readFile(settings.value("note").toString()));
        fc->changPath();
    }
}
QString getHostMacAddress(){
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}
QString mac=getHostMacAddress();
QString HttpGet(QString link){
    QUrl url(host+link);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}
QString httpGet(QString link){
    QUrl url(link);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}
QString httpPost(QString link,QByteArray postData){
    QUrl url(host+link);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply= networkManager->post(request,postData);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}
QString httpPostJson(QString jsonDoc) {
    QUrl url("http://localhost:8765/");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->post(request, jsonDoc.toUtf8());
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}
bool noteVisible(){
    return note->isVisible();
}
void openNoteEditor(){
    note->setNote(fc->readFile(settings.value("note").toString()));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    note=new Note;

    if(argc==2){
        QString ntc = QString::fromUtf8(argv[1]);
        QUrl url(ntc);
        QUrlQuery query(url);
        QString noteUrl = query.queryItemValue("url");
        QString text = query.queryItemValue("text");

        if(!text.isEmpty()){
            QTimer::singleShot(500, [=]() {
             note->highlight(fc->readFile(noteUrl),text);
            });

        }

    }
    networkManager=new QNetworkAccessManager;
    fc=new MainWindow;
    if(argc!=2){fc->showNormal();}

    QSettings reg("HKEY_CURRENT_USER\\Software\\Classes\\ntc", QSettings::NativeFormat);
    reg.setValue("Default","URL:ntc");
    reg.setValue("URL Protocol","");
    reg.setValue("shell/open/command/Default",a.applicationFilePath().replace("/","\\")+" %1");




    return a.exec();
}
