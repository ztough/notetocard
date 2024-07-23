#include "grammar.h"
#include "ui_grammar.h"

Grammar::Grammar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Grammar)
{
    ui->setupUi(this);
    setWindowTitle("制卡语法");

    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

     setWindowIcon(QIcon(":/img/logo.png"));
    setMaximumSize(width(),height());
    setMinimumSize(width(),height());
    ui->lineEdit->setText(settings.value("qa","[?？]$").toString());
    ui->lineEdit_2->setText(settings.value("cloze","\\{\\{(.*?)\\}\\}").toString());
    ui->lineEdit_3->setText(settings.value("choice","\\|(.*)$").toString());
    ui->lineEdit_4->setText(settings.value("judge","\\|\\|(.*)$").toString());
}

Grammar::~Grammar()
{
    delete ui;
}



void Grammar::on_lineEdit_textEdited(const QString &arg1)
{
    settings.setValue("qa",arg1);
    updateTree();
}


void Grammar::on_lineEdit_2_textEdited(const QString &arg1)
{
    settings.setValue("cloze",arg1);
    updateTree();
}


void Grammar::on_lineEdit_3_textEdited(const QString &arg1)
{
    settings.setValue("choice",arg1);
    updateTree();
}


void Grammar::on_lineEdit_4_textEdited(const QString &arg1)
{
    settings.setValue("judge",arg1);
updateTree();
}


void Grammar::on_lineEdit_textChanged(const QString &arg1)
{
    settings.setValue("qa",arg1);

}


void Grammar::on_lineEdit_2_textChanged(const QString &arg1)
{
    settings.setValue("cloze",arg1);

}


void Grammar::on_lineEdit_3_textChanged(const QString &arg1)
{
    settings.setValue("choice",arg1);

}


void Grammar::on_lineEdit_4_textChanged(const QString &arg1)
{
    settings.setValue("judge",arg1);

}

