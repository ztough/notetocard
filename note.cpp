#include "note.h"
#include "notewebenginepage.h"
#include "ui_note.h"
#include <QWebEngineSettings>
#include <Bridge.h>
#include <QDir>
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

void Note::setNote(QString note)
{

    ui->webEngineView->page()->runJavaScript(QString("setNote(`%1`)").arg(note.replace("`", "\\`")));

    showNormal();


}

void Note::highlight(QString note,QString text)
{
    ui->webEngineView->page()->runJavaScript(QString("setNote(`%1`)").arg(note.replace("`", "\\`")));
    ui->webEngineView->page()->runJavaScript(QString("highlightText(`%1`)").arg(text.replace("`", "\\`")));
    showNormal();

}

void Note::insertNote(QString note)
{
    ui->webEngineView->page()->runJavaScript(QString("insertNote(`%1`)").arg(note.replace("`", "\\`")));
}

Note::Note(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Note)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/img/logo.png"));
    setWindowTitle("笔记编辑器");
    if(!settings.value("note_pos").isNull()){
        QPoint pos = settings.value("note_pos").toPoint();
        move(pos);
    }
    if(!settings.value("note_size").isNull()){
        QSize size = settings.value("note_size").toSize();
        resize(size);
    }
    QWebChannel *channel = new QWebChannel(this);
    Bridge* bridge = new Bridge;
    channel->registerObject(QStringLiteral("bridge"), bridge);
    NoteWebEnginePage *page =new NoteWebEnginePage(this);
    ui->webEngineView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    page->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->webEngineView->setPage(page);
    ui->webEngineView->page()->setWebChannel(channel);

    ui->webEngineView->page()->load(QDir(QCoreApplication::applicationDirPath()).filePath("NoteEditor/index.html"));
}

Note::~Note()
{
    delete ui;
}
