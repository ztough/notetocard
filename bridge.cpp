#include "bridge.h"
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <QTimer>
Bridge::Bridge() {}
void Bridge::saveNote(QString note)
{
    note=note.trimmed();
    if(note.isEmpty()){return;}
    QFile file(settings.value("note").toString());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    }
    QTextStream out(&file);
    out << note;
    file.close();
}

void Bridge::flashcard(QString note)
{

    if(!note.trimmed().isEmpty()){
        fc->setTreeView(note);

    }
    fc->showNormal();
}

void Bridge::sm(QString note)
{


    if(!note.trimmed().isEmpty()){
        fc->setTreeView(note);
        fc->exportSm();
    }
}

void Bridge::anki(QString note)
{

    if(!note.trimmed().isEmpty()){
        fc->setTreeView(note);
        fc->note2anki();
    }
}

void Bridge::html(QString note)
{

    if(!note.trimmed().isEmpty()){
        fc->setTreeView(note);
        fc->htmlTemplate();
    }
}

void Bridge::checkTextItem(QString note,QString item)
{
    note=note.trimmed();
    qDebug()<<item;
    if(fc->isVisible()&&noteVisible()&&!note.isEmpty()){
        if(fc->currentNote==note){
            fc->clickItem(item);
        }else{
            fc->setTreeView(note.trimmed());
            fc->clickItem(item);
        }

    }
}
