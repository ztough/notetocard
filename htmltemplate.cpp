#include "htmltemplate.h"
#include "ui_htmltemplate.h"
#include <QRegularExpression>
#include <QMessageBox>

QString HtmlTemplate::smFront()
{
    return ui->textEdit->toPlainText();

}

QString HtmlTemplate::ankiFront()

{
    QRegularExpression re("<body[^>]*>(.*?)<\\/body>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(ui->textEdit->toPlainText());
    QString bodyContent;
    if (match.hasMatch()) {
        bodyContent= match.captured(1);
    }
    return  bodyContent;
}

QString HtmlTemplate::smBack()
{
    return ui->textEdit_2->toPlainText();
}

QString HtmlTemplate::ankiBack()
{
    QRegularExpression re("<body[^>]*>(.*?)<\\/body>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(ui->textEdit_2->toPlainText());
    QString bodyContent;
    if (match.hasMatch()) {
        bodyContent= match.captured(1);
    }
    return  bodyContent;
}

HtmlTemplate::HtmlTemplate(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HtmlTemplate)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowIcon(QIcon(":/img/logo.png"));
    setWindowTitle("自定义模板");
    if(!settings.value("html_pos").isNull()){
        QPoint pos = settings.value("html_pos").toPoint();
        move(pos);
    }
    if(!settings.value("html_size").isNull()){
        QSize size = settings.value("html_size").toSize();
        resize(size);
    }

    ui->textEdit->setPlainText(settings.value("front",front).toString());
    ui->textEdit_2->setPlainText(settings.value("back",back).toString());

    ui->textEdit->setAcceptRichText(false);
    ui->textEdit_2->setAcceptRichText(false);

}

HtmlTemplate::~HtmlTemplate()
{
    delete ui;
}

void HtmlTemplate::on_pushButton_clicked()
{
    settings.setValue("front",ui->textEdit->toPlainText());
    settings.setValue("back",ui->textEdit_2->toPlainText());
    QMessageBox::information(this, "提示", "保存成功", QMessageBox::Ok);
}


void HtmlTemplate::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认", "你确定要重置为默认模板吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->textEdit->setPlainText(front);
        ui->textEdit_2->setPlainText(back);
        settings.setValue("front",ui->textEdit->toPlainText());
        settings.setValue("back",ui->textEdit_2->toPlainText());

        QMessageBox::information(this, "提示", "重置成功", QMessageBox::Ok);
    }

}


void HtmlTemplate::on_pushButton_3_clicked()
{
    clickSelf();
}

