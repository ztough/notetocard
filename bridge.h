#ifndef BRIDGE_H
#define BRIDGE_H

#include "mainwindow.h"

#include <QObject>
#include <QMessageBox>

extern void simulateCtrlV();

extern MainWindow *fc;
extern QString text;
extern bool noteVisible();
class Bridge:public QObject
{
    Q_OBJECT
public:
    Bridge();
public slots:

    void saveNote(QString note);
    void flashcard(QString note);
    void sm(QString note);
    void anki(QString note);
    void html(QString note);
    void checkTextItem(QString note,QString item);
};

#endif // BRIDGE_H
