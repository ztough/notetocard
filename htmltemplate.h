#ifndef HTMLTEMPLATE_H
#define HTMLTEMPLATE_H
#include <QSettings>
#include <QWidget>
#include <QMouseEvent>
namespace Ui {
class HtmlTemplate;
}
extern QSettings settings;
extern void clickSelf();
class HtmlTemplate : public QWidget
{
    Q_OBJECT

public:
    void moveEvent(QMoveEvent *event) override {
        settings.setValue("html_pos", this->pos());
    }
    void resizeEvent(QResizeEvent *event) override {
        settings.setValue("html_size", this->size());
    }
    /*void closeEvent(QCloseEvent *event) override {
        event->ignore();
        this->hide();
    }*/
    QString front=R"(<!DOCTYPE html>
<html>
<body class="card">
    <div>
        <h2 id="front"></h2>
        <h3  id="path"></h3>
      <h4 id="link"></h4>
      </div>
      <script id="ntcscript">
       var front={{Front}}.join("</br>");
      front=front.replace(/!\[(.*?)\]\((.*?)\)/g,'<img src="$2" alt="$1"/>');
      front=front.replace(/\[(.*?)\]\((.*?)\)/g,'<a href="$2">$1</a>');
       document.getElementById("front").innerHTML=front;
       document.getElementById("path").innerHTML={{Path}}.join(" → ");
       document.getElementById("link").innerHTML="<a href='{{Link}}'>跳转原文</a>";
      </script>
</body>
</html>)";
    QString back=R"(<!DOCTYPE html>
<html>
<body class="card">
    {{FrontSide}}
<hr>
    <div>
      <h3 id="back"></h3>
    </div>
    <script id="ntcscript">
    var back={{Back}}.join("</br>");
    back=back.replace(/!\[(.*?)\]\((.*?)\)/g,'<img src="$2" alt="$1"/>');
    back=back.replace(/\[(.*?)\]\((.*?)\)/g,'<a href="$2">$1</a>');
    document.getElementById("back").innerHTML=back;
    </script>
</body>
</html>)";
    QString smFront();
    QString ankiFront();
    QString smBack();
    QString ankiBack();
    explicit HtmlTemplate(QWidget *parent = nullptr);
    ~HtmlTemplate();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::HtmlTemplate *ui;
};

#endif // HTMLTEMPLATE_H
