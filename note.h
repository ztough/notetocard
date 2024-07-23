#ifndef NOTE_H
#define NOTE_H

#include <QWidget>
#include <QSettings>
#include <QMouseEvent>
namespace Ui {
class Note;
}
extern QSettings settings;
class Note : public QWidget
{
    Q_OBJECT

public:
    void moveEvent(QMoveEvent *event) override {
        settings.setValue("note_pos", this->pos());
    }
    void resizeEvent(QResizeEvent *event) override {
        settings.setValue("note_size", this->size());
    }
   /* void closeEvent(QCloseEvent *event) override {
        event->ignore();
        this->hide();
    }*/
    void setNote(QString note);
    void highlight(QString note,QString text);
    void insertNote(QString note);
    explicit Note(QWidget *parent = nullptr);
    ~Note();

private:
    Ui::Note *ui;
};

#endif // NOTE_H
