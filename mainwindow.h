#ifndef FLASHCARD_H
#define FLASHCARD_H

#include "htmltemplate.h"
#include "grammar.h"

#include <QMainWindow>
#include <QSettings>
#include <QMouseEvent>
#include <QStandardItem>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
namespace Ui {
class MainWindow;
}
extern QSettings settings;
extern QString httpPostJson(QString jsonDoc);
extern bool noteVisible();
extern void openNoteEditor();
extern QString host;
extern QString localHost;
extern QString httpGet(QString link);
extern QNetworkAccessManager *networkManager;
class Card{
public:
    const QString getFront() {
        front.replaceInStrings("'","\\'");

        QString frontStr=front.join("','");

        // frontStr.replace(QRegularExpression("!\\[(.*?)\\]\\((.*?)\\)"),"<img src=\"\\2\" alt=\"\\1\"/>");

        //frontStr.replace(QRegularExpression("\\[(.*?)\\]\\((.*?)\\)"),"<a href=\"\\2\">\\1</a>");


        return "['"+frontStr+"']"; }
    void setFront(const QStringList& value) { front = value; }

    const QString getBack()  {
        back.replaceInStrings("'","\\'");
        QString backStr=back.join("','");
        //  backStr.replace(QRegularExpression("!\\[(.*?)\\]\\((.*?)\\)"),"<img src=\"\\2\" alt=\"\\1\"/>");
        //  backStr.replace(QRegularExpression("\\[(.*?)\\]\\((.*?)\\)"),"<a href=\"\\2\">\\1</a>");

        return "['"+backStr+"']";  }

    const QString getPath()  {
        path.replaceInStrings("'","\\'");
        QString pathStr="['"+path.join("','")+"']";
        return pathStr;  }

public:

    QStringList front;
    QStringList back;
    QStringList path;
    QString link;
    QString type;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    Grammar *grammar;
    QString currentNote;
    QString index;
    QString readFile(const QString& filePath);
    QFileSystemWatcher watcher;
    void changPath();
    void clickItem(QString item);
    void clickSelf();
    QSet<QString> deckSet;
    QSet<qint64> cardSet;
    QString rootDeckName;
    QStandardItemModel *model=nullptr;
    QMap<QStandardItem *, Card> cardMap;
    int calculateIndent(const QString &line);
    void createTreeModel(const QStringList &lines, QStandardItem *parent, int &currentLine, int currentIndent);
    QStandardItemModel *createModelFromString(const QString &text);
    void traverseModel(QStandardItem *item);
    void traverseAnki(QStandardItem *item);
    void traverseModelTopic(QStandardItem *item);
    QStringList buildItemPath(QStandardItem *item);
    void saveIndex(QStandardItem *item,QStandardItem *current, QString path = "");
    void setIndex(QStandardItem *item,QString path = "");
    void exportItem(QStandardItem *item, QXmlStreamWriter &xmlWriter, int &idCounter);
    void setTreeView(QString note);
    void deleteDeck();
    void deleteCard();
    QString zhuanyi(QString str);
    QString formatFront(QStandardItem *item);
    QString formatBack(QStandardItem *item);
    QString formatFrontResult(QStandardItem *item);
    QString formatBackResult(QStandardItem *item);
    void checkTextItem(QStandardItem *item,QString text);

    HtmlTemplate *html;

    void checkUpdate();
    void ankiModel();
    void moveEvent(QMoveEvent *event) override {
        settings.setValue("fc_pos", this->pos());
    }
    void resizeEvent(QResizeEvent *event) override {
        settings.setValue("fc_size", this->size());
    }
    void closeEvent(QCloseEvent *event) override {
        QApplication::exit(0);
    }

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_treeView_clicked(const QModelIndex &index);
    void exportSm();
    void note2anki();
    void htmlTemplate();
    void openNote();
    void markdown();
    void gram();
    void qq();
    void jiaochen();
    void gw();
    void updateCheck();
private:
    Ui::MainWindow *ui;
};

#endif // FLASHCARD_H
