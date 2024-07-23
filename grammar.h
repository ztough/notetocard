#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QWidget>
#include <QSettings>
namespace Ui {
class Grammar;
}
extern QSettings settings;
extern void updateTree();
class Grammar : public QWidget
{
    Q_OBJECT

public:
    explicit Grammar(QWidget *parent = nullptr);
    ~Grammar();

private slots:


    void on_lineEdit_textEdited(const QString &arg1);

    void on_lineEdit_2_textEdited(const QString &arg1);

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_lineEdit_4_textEdited(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_4_textChanged(const QString &arg1);

private:
    Ui::Grammar *ui;
};

#endif // GRAMMAR_H
