
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QRegularExpression>
#include <QString>
#include <QTextDocument>
#include <QThread>
QString MainWindow::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();

    file.close();
    return content;
}

void MainWindow::changPath()
{
    watcher.removePaths(watcher.files());
    watcher.addPath(settings.value("note").toString());
}

void MainWindow::clickItem(QString item)
{
    if(noteVisible()){
        checkTextItem(model->invisibleRootItem(),item);
    }
    if(ui->treeView->currentIndex().isValid()){
        on_treeView_clicked(ui->treeView->currentIndex());
    }


}

void MainWindow::clickSelf()
{
    if(ui->treeView->currentIndex().isValid()){
        on_treeView_clicked(ui->treeView->currentIndex());
    }

}

int MainWindow::calculateIndent(const QString &line)
{
    int indent = 0;
    for (int i = 0; i < line.length(); ++i) {
        if (line[i] == ' ') {
            ++indent;
        } else if (line[i] == '\t') {
            indent += 4;
        } else if (line[i] == '#') {
            ++indent;
        } else {
            break;
        }
    }
    return indent;
}

void MainWindow::createTreeModel(const QStringList &lines, QStandardItem *parent, int &currentLine, int currentIndent)
{
    while (currentLine < lines.size()) {
        QString line = lines[currentLine];
        int lineIndent = calculateIndent(line);

        if (lineIndent == currentIndent) {
            QStandardItem *item = new QStandardItem(line.trimmed().remove(QRegularExpression("^[+\\-*#]* ")));

            parent->appendRow(item);
            ++currentLine;

        } else if (lineIndent > currentIndent) {
            QStandardItem *lastItem = parent->child(parent->rowCount() - 1);
            if(!lastItem){
                lastItem=parent;
            }
            createTreeModel(lines, lastItem, currentLine, lineIndent);
        } else {
            return;
        }
    }
}

QStandardItemModel *MainWindow::createModelFromString(const QString &text)
{
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    QStandardItemModel *model = new QStandardItemModel;
    QStandardItem *rootItem = model->invisibleRootItem();

    int currentLine = 0;
    createTreeModel(lines, rootItem, currentLine, 0);

    return model;
}

void MainWindow::traverseModel(QStandardItem *item)
{
    if (!item) return;
    bool flag=false;
    QString link="ntc://go?url=%1&text=%2";
    link=link.arg(settings.value("note").toString()).arg(item->text());
    QString text=item->text();
    QRegularExpression re(settings.value("qa").toString());
    re.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(text);
    if(match.hasMatch()&&flag==false){
        flag=true;
        QIcon icon(":/img/items.png");
        item->setIcon(icon);
        item->setData("item");


        QStringList sl;
        for (int i = 0; i < item->rowCount(); ++i) {
            sl<<item->child(i)->text();
        }
        Card card;
        card.front<<text;
        card.back=sl;
        card.path=buildItemPath(item);
        card.link=link;
        card.type="qa";
        cardMap.insert(item,card);

    }
    re.setPattern(settings.value("judge").toString());
    match = re.match(text);

    if(match.hasMatch()&&flag==false){
        flag=true;
        QIcon icon(":/img/items.png");
        item->setIcon(icon);
        item->setData("item");
        Card card;
        card.front<<item->text().replace("||"+match.captured(1),"");
        card.back<<match.captured(1);
        card.path=buildItemPath(item);
        card.link=link;
        card.type="judge";
        cardMap.insert(item,card);
    }
    re.setPattern(settings.value("choice").toString());
    match = re.match(text);
    if(match.hasMatch()&&flag==false){
        flag=true;
        QIcon icon(":/img/items.png");
        item->setIcon(icon);
        item->setData("item");
        QStringList sl;
        for (int i = 0; i < item->rowCount(); ++i) {
            sl<<item->child(i)->text();
        }
        Card card;
        card.front<<text.replace(match.captured(0),"")<<sl;
        card.back<<match.captured(1);
        card.path=buildItemPath(item);
        card.link=link;
        card.type="choice";
        cardMap.insert(item,card);
    }
    re.setPattern(settings.value("cloze").toString());
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(text);
    if(matchIterator.hasNext()&&flag==false){
        flag=true;
        QIcon icon(":/img/items.png");
        item->setIcon(icon);
        item->setData("item");
        QStringList ls;
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            QString matchedText = match.captured(1);
            ls<< matchedText;

        }
        text.replace(QRegularExpression(settings.value("cloze").toString()),"[...]");
        Card card;
        card.front<<text;
        card.back<<ls;
        card.path=buildItemPath(item);
        card.link=link;
        card.type="cloze";
        cardMap.insert(item,card);
    }
    for (int i = 0; i < item->rowCount(); ++i) {
        traverseModel(item->child(i));
    }
}

void MainWindow::traverseAnki(QStandardItem *item)
{
    if (!item) return;

    if(item->data().toString()=="item"){

        QString deckName=rootDeckName +buildItemPath(item).join("::");

        deckSet<<deckName+"::";

        QString createDeck =R"({
    "action": "createDeck",
    "version": 6,
    "params": {
        "deck": "%1"
    }
})";

        QString addNote = R"({
    "action": "addNote",
    "version": 6,
    "params": {
        "note": {
            "deckName": "%1",
            "modelName": "NoteToCard",
            "fields": {
                "Front": "%2",
                "Back": "%3","Path": "%4","Link":"%5","Type":"%6"
            },
            "options": {
                "allowDuplicate": false,
                "duplicateScope": "deck"
            }
        }
    }
})";
        QString findNotes =R"({
    "action": "findNotes",
    "version": 6,
    "params": {
        "query": "Front:%1"
    }
})";
        QString changeDeck=R"(
{
    "action": "changeDeck",
    "version": 6,
    "params": {
        "cards": [%1],
        "deck": "%2"
    }
}
)";
        QString updateNote=R"(
{
    "action": "updateNote",
    "version": 6,
    "params": {
        "note": {
            "id": %1,
            "fields": {
                "Front": "%2",
                "Back": "%3",
                "Path": "%4","Link":"%5","Type":"%6"
            }
        }
    }
})";
        httpPostJson(createDeck.arg(deckName));
        QString res= httpPostJson(findNotes.arg(cardMap.value(item).getFront()));
        QJsonDocument doc=QJsonDocument::fromJson(res.toUtf8());
        if(doc.object().value("result").toArray().size()>0){
            cardSet<<doc.object().value("result").toArray().at(0).toInteger();
            httpPostJson(updateNote.arg(doc.object().value("result").toArray().at(0).toInteger()).arg(cardMap.value(item).getFront()).arg(cardMap.value(item).getBack()).arg(cardMap.value(item).getPath()).arg(cardMap.value(item).link).arg(cardMap.value(item).type));
            httpPostJson(changeDeck.arg(doc.object().value("result").toArray().at(0).toInteger()).arg(deckName));
        }else{
            res= httpPostJson(addNote.arg(deckName).arg(cardMap.value(item).getFront()).arg(cardMap.value(item).getBack()).arg(cardMap.value(item).getPath()).arg(cardMap.value(item).link).arg(cardMap.value(item).type));
            doc=QJsonDocument::fromJson(res.toUtf8());
            cardSet<<doc.object().value("result").toInteger();
        }

    }


    for (int i = 0; i < item->rowCount(); ++i) {
        traverseAnki(item->child(i));
    }
}

void MainWindow::traverseModelTopic(QStandardItem *item)
{
    if (!item) return;
    if(item->icon().isNull()){

        QIcon icon(":/img/topic.png");
        item->setIcon(icon);
        item->setData("topic");
    }
    for (int i = 0; i < item->rowCount(); ++i) {
        traverseModelTopic(item->child(i));
    }
}

QStringList MainWindow::buildItemPath(QStandardItem *item)
{
    QStringList pathParts;
    QStandardItem *current = item->parent();

    while (current) {
        pathParts.prepend(current->text());
        if(current->icon().isNull()){
            QIcon icon(":/img/folder.png");
            current->setIcon(icon);

            current->setData("folder");
        }

        current = current->parent();
    }
    pathParts.prepend(model->horizontalHeaderItem(0)->text());
    return pathParts;
}

void MainWindow::saveIndex(QStandardItem *item,QStandardItem *current, QString path)
{
    if (!item) {
        return;
    }


    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i) {

        QStandardItem *childItem = item->child(i);


        QString currentPath = path + QString::number(i + 1);

        if(current==childItem){
            index=currentPath;
            return;
        }

        saveIndex(childItem,current, currentPath + ".");
    }
}

void MainWindow::setIndex(QStandardItem *item, QString path)
{
    if (!item) {
        return;
    }


    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i) {

        QStandardItem *childItem = item->child(i);

        QString currentPath = path + QString::number(i + 1);

        if(index==currentPath){
            ui->treeView->setCurrentIndex(model->indexFromItem(childItem));
            return;
        }

        setIndex(childItem, currentPath + ".");
    }
}

void MainWindow::exportItem(QStandardItem *item, QXmlStreamWriter &xmlWriter, int &idCounter)
{

    for (int row = 0; row < item->rowCount(); ++row) {
        QStandardItem *childItem = item->child(row);
        xmlWriter.writeStartElement("SuperMemoElement");
        xmlWriter.writeTextElement("ID", QString::number(idCounter++));

        if(childItem->data().toString()=="item"){
            xmlWriter.writeTextElement("Type", "Item");
            xmlWriter.writeStartElement("Content");
            xmlWriter.writeTextElement("Question", formatFrontResult(childItem));
            xmlWriter.writeTextElement("Answer", formatBackResult(childItem));
            xmlWriter.writeEndElement();
        }
        if(childItem->data().toString()=="topic"){
            xmlWriter.writeTextElement("Type", "Topic");
            xmlWriter.writeStartElement("Content");
            xmlWriter.writeTextElement("Question", childItem->text());
            xmlWriter.writeEndElement();
        }
        if(childItem->data().toString()=="folder"){
            xmlWriter.writeTextElement("Title", childItem->text());
            xmlWriter.writeTextElement("Type", "Topic");
        }

        if (childItem->hasChildren()) {
            exportItem(childItem, xmlWriter, idCounter);
        }
        xmlWriter.writeEndElement();
    }
}

void MainWindow::setTreeView(QString note)
{

    note=note.trimmed().remove(QRegularExpression("\u200B|\u200C|\u200D|\u200E|\u200F|\u2028|\u2029|\u2060"));
    note=note.trimmed();
    if(note.isEmpty()){return;}
    currentNote=note;
    cardMap.clear();

    model =createModelFromString(note);

    model->setHeaderData(0, Qt::Horizontal, QFileInfo(settings.value("note").toString()).baseName());
    QIcon icon(":/img/concept.png");
    model->horizontalHeaderItem(0)->setIcon(icon);
    ui->treeView->setModel(model);
    ui->treeView->expandAll();
    traverseModel(model->invisibleRootItem());
    traverseModelTopic(model->invisibleRootItem());

    if(!noteVisible()){
        setIndex(model->invisibleRootItem());
        if(!ui->treeView->currentIndex().isValid()){
            ui->treeView->setCurrentIndex(model->index(0,0));
        }
        if(ui->treeView->currentIndex().isValid()){
            on_treeView_clicked(ui->treeView->currentIndex());
        }

    }

}

void MainWindow::deleteDeck()
{

    QString deckNames =R"({
    "action": "deckNames",
    "version": 6
})";
    QString res= httpPostJson(deckNames);
    QJsonDocument doc=QJsonDocument::fromJson(res.toUtf8());

    auto arr=doc.object().value("result").toArray();
    for (const QJsonValue &value : arr) {


        if(value.toString().startsWith(rootDeckName+model->horizontalHeaderItem(0)->text()+"::")){
            bool flag=false;
            for(auto str:deckSet){

                if(str.startsWith(value.toString()+"::")){
                    flag=true;
                    break;
                }
            }
            if(flag==false){
                QString deleteDecks =R"({
    "action": "deleteDecks",
    "version": 6,
    "params": {
        "decks": ["%1"],
        "cardsToo": true
    }
})";
                httpPostJson(deleteDecks.arg(value.toString()));
            }

        }


    }

}

void MainWindow::deleteCard()
{
    QString findNotes =R"({
    "action": "findNotes",
    "version": 6,
    "params": {
        "query": "\"deck:%1\""
    }
})";

    QString res= httpPostJson(findNotes.arg(rootDeckName+model->horizontalHeaderItem(0)->text()));
    QJsonDocument doc=QJsonDocument::fromJson(res.toUtf8());
    auto arr=doc.object().value("result").toArray();
    for (const QJsonValue &value : arr) {
        if(!cardSet.contains(value.toInteger())){
            QString deleteNotes =R"({
    "action": "deleteNotes",
    "version": 6,
    "params": {
        "notes": [%1]
    }
})";
            httpPostJson(deleteNotes.arg(value.toInteger()));
        }

    }

}

QString MainWindow::zhuanyi(QString str)
{
    str.replace("\\", "\\\\");
    str.replace("\"", "\\\"");
    str.replace("\r", "\\r");
    str.replace("\n", "\\n");
    str.replace("\t", "\\t");
    str.replace("\b", "\\b");
    str.replace("\f", "\\f");
    return str;
}

QString MainWindow::formatFront(QStandardItem *item)
{
    QString frontCard=html->smFront().replace("{{Front}}",cardMap.value(item).getFront()).replace("{{Back}}",cardMap.value(item).getBack()).replace("{{Path}}",cardMap.value(item).getPath()).replace("{{Link}}",cardMap.value(item).link).replace("{{Type}}",cardMap.value(item).type).replace("{{FrontSide}}","");

    return frontCard;
}

QString MainWindow::formatBack(QStandardItem *item)
{
    QString backCard=html->smBack().replace("{{Front}}",cardMap.value(item).getFront()).replace("{{Back}}",cardMap.value(item).getBack()).replace("{{Path}}",cardMap.value(item).getPath()).replace("{{Link}}",cardMap.value(item).link).replace("{{Type}}",cardMap.value(item).type).replace("{{FrontSide}}","");

    return backCard;
}
QString MainWindow::formatFrontResult(QStandardItem *item)
{
    QWebEngineView view;
    QWebEnginePage *page = view.page();
    QString frontCard=html->smFront().replace("{{Front}}",cardMap.value(item).getFront()).replace("{{Back}}",cardMap.value(item).getBack()).replace("{{Path}}",cardMap.value(item).getPath()).replace("{{Link}}",cardMap.value(item).link).replace("{{Type}}",cardMap.value(item).type).replace("{{FrontSide}}","");
    QString htmlResult;
    QEventLoop loop;
    QObject::connect(page, &QWebEnginePage::loadFinished, [&]() {
        page->toHtml([&](const QString& html) {
            htmlResult = html;
            loop.quit();
        });
    });
    view.setHtml(frontCard);
    loop.exec();
    htmlResult.replace(QRegularExpression("<script id=\"ntcscript\">.*?<\\/script>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption),"");
    // htmlResult.replace(QRegularExpression("<style id=\"cnstyle\">.*?<\\/style>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption),"");
    return htmlResult;
}

QString MainWindow::formatBackResult(QStandardItem *item)
{


    QWebEngineView view;
    QWebEnginePage *page = view.page();
    QString backCard=html->smBack().replace("{{Front}}",cardMap.value(item).getFront()).replace("{{Back}}",cardMap.value(item).getBack()).replace("{{Path}}",cardMap.value(item).getPath()).replace("{{Link}}",cardMap.value(item).link).replace("{{Type}}",cardMap.value(item).type).replace("{{FrontSide}}","");
    QString htmlResult;
    QEventLoop loop;
    QObject::connect(page, &QWebEnginePage::loadFinished, [&]() {
        page->toHtml([&](const QString& html) {
            htmlResult = html;
            loop.quit();
        });
    });
    view.setHtml(backCard);
    loop.exec();
    htmlResult.replace(QRegularExpression("<script id=\"ntcscript\">.*?<\\/script>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption),"");
    // htmlResult.replace(QRegularExpression("<style id=\"cnstyle\">.*?<\\/style>", QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption),"");
    return htmlResult;

}

void MainWindow::checkTextItem(QStandardItem *item,QString text)
{
    if (!item) {
        return;
    }
    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = item->child(i);
        if(childItem->text()==text){
            if(childItem->data().toString()=="item"){
                ui->treeView->setCurrentIndex(model->indexFromItem(childItem));
            }else if(childItem->parent()&&childItem->parent()->data().toString()=="item"){
                ui->treeView->setCurrentIndex(model->indexFromItem(childItem->parent()));
            }else{
                ui->treeView->setCurrentIndex(model->indexFromItem(childItem));
            }
            return;
        }
        checkTextItem(childItem,text);
    }
}

void MainWindow::gw()
{
    QUrl url("https://ztough.cn");
    QDesktopServices::openUrl(url);
}
void MainWindow::updateCheck()
{
    QString update=httpGet("https://ztough-1308253351.cos.ap-nanjing.myqcloud.com/notetocard.txt");

    if(update.isEmpty()){
        QMessageBox::critical(this, "错误", "当前网络异常无法更新检测", QMessageBox::Ok);
        return;
    }
    QString filePath=QDir(QCoreApplication::applicationDirPath()).filePath("NoteToCard.exe");
    QFileInfo fileInfo(filePath);

    QDateTime newVersion=QDateTime::fromString(update, "yyyy-MM-dd HH:mm:ss");
    QDateTime nowVersion=fileInfo.lastModified();
    if(newVersion>nowVersion){
        QMessageBox::information(this, "提示", "发现新版本，确认后将进行下载，退出程序后（无需卸载）覆盖安装即可。", QMessageBox::Ok);
        QUrl url("https://vip.123pan.cn/1826326401/NoteToCard/NoteToCardSetup.exe");
        QDesktopServices::openUrl(url);
        QApplication::exit();

    }else{
        QMessageBox::information(this, "提示", "当前是最新版本", QMessageBox::Ok);

    }
}

void MainWindow::checkUpdate()
{
    QUrl url("https://ztough-1308253351.cos.ap-nanjing.myqcloud.com/notetocard.txt");
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString update=httpGet("https://ztough-1308253351.cos.ap-nanjing.myqcloud.com/notetocard.txt");
            QString filePath=QDir(QCoreApplication::applicationDirPath()).filePath("NoteToCard.exe");
            QFileInfo fileInfo(filePath);
            QDateTime newVersion=QDateTime::fromString(update, "yyyy-MM-dd HH:mm:ss");
            QDateTime nowVersion=fileInfo.lastModified();
            if(newVersion>nowVersion){
                setWindowTitle("NoteToCard笔记卡 检测到新版本，请去官网下载最新版。");

            }
        }

    });
}
void MainWindow::ankiModel()
{
    QString findModelsByName=R"({
    "action": "findModelsByName",
    "version": 6,
    "params": {
        "modelNames": ["NoteToCard"]
    }
})";
    QString createModel=R"(
{
    "action": "createModel",
    "version": 6,
    "params": {
        "modelName": "NoteToCard",
        "inOrderFields": ["Front", "Back", "Path","Link","Type"],
        "css": "",
        "isCloze": false,
        "cardTemplates": [
            {
                "Name": "NoteToCard",
                "Front": "%1",
                "Back": "%2"
            }
        ]
    }
}
)";

    QString updateModelTemplates=R"(
{
    "action": "updateModelTemplates",
    "version": 6,
    "params": {
        "model": {
            "name": "NoteToCard",
            "templates": {
                "NoteToCard": {
                    "Front": "%1",
                    "Back": "%2"
                }
            }
        }
    }
}
)";
    /* QString updateModelStyling=R"({
    "action": "updateModelStyling",
    "version": 6,
    "params": {
        "model": {
            "name": "NoteToCard",
            "css": "%1"
        }
    }
})";*/
    QString res=httpPostJson(findModelsByName);

    QJsonDocument doc=QJsonDocument::fromJson(res.toUtf8());
    if(doc.object().value("result").toArray().size()<=0){
        httpPostJson(createModel.arg(zhuanyi(html->ankiFront())).arg(zhuanyi(html->ankiBack())));
    }else{

        httpPostJson(updateModelTemplates.arg(zhuanyi(html->ankiFront())).arg(zhuanyi(html->ankiBack())));
        //  httpPostJson(updateModelStyling.arg(zhuanyi(html->css)));

    }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/img/logo.png"));

    setWindowTitle("NoteToCard笔记卡");
    if(!settings.value("fc_pos").isNull()){
        QPoint pos = settings.value("fc_pos").toPoint();
        move(pos);
    }
    if(!settings.value("fc_size").isNull()){
        QSize size = settings.value("fc_size").toSize();
        resize(size);
    }
    grammar=new Grammar;
    ui->splitter_2->setStretchFactor(0, 1);
    ui->splitter_2->setStretchFactor(1, 4);
    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, [&](const QString &path) {
        if(!noteVisible()){
            QString text=readFile(path);
            if(!text.isEmpty()){
                if(model!=nullptr){
                    delete model;
                }
                ui->treeView->setModel(nullptr);

                setTreeView(readFile(path));
            }
        }

    });
    html=new HtmlTemplate;



    if(!settings.value("note").toString().isEmpty()){
        setTreeView(readFile(settings.value("note").toString()));
        changPath();
    }

    checkUpdate();

}


MainWindow::~MainWindow()
{
    delete ui;
    delete html;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{

    QStandardItem *item = model->itemFromIndex(index);

    if(cardMap.contains(item)){
        ui->webEngineView_2->setVisible(true);
        ui->webEngineView->setHtml(formatFront(item));
        ui->webEngineView_2->setHtml(formatBack(item));
    }else{
        if(item->data().toString()=="topic"){
            ui->webEngineView_2->setVisible(false);
            ui->webEngineView->setHtml(item->text());
        }else{
            ui->webEngineView_2->setVisible(false);
            ui->webEngineView->setHtml("<style>body{background-color:whitesmoke;}</style>");
        }

    }
    saveIndex(model->invisibleRootItem(), item,"");
}

void MainWindow::exportSm()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QObject::tr("保存Sm卡组"),
                                                    "",
                                                    QObject::tr("Sm卡组(*.xml)"));
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("SuperMemoCollection");

    xmlWriter.writeStartElement("SuperMemoElement");
    xmlWriter.writeTextElement("ID", QString::number(1));
    xmlWriter.writeTextElement("Title", rootDeckName+model->horizontalHeaderItem(0)->text());
    xmlWriter.writeTextElement("Type", "Concept");

    int idCounter = 2;
    exportItem(model->invisibleRootItem(), xmlWriter, idCounter);

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();
    QMessageBox::information(this, "提示", "导出成功", QMessageBox::Ok);
}

void MainWindow::note2anki()
{

    QString text = QInputDialog::getText(this, "请输入完整卡组名称 如x::x::x", "将生成在这个卡组之下，root为根目录", QLineEdit::Normal, settings.value("deck","root").toString());
    QString rootDeckNames;
    if(text.isEmpty()){
        return;
    }else{
        if(text=="root"){
            rootDeckName="";
        }else {
            rootDeckNames= rootDeckName;
            rootDeckName+=text+"::";
        }

    }
    ankiModel();
    settings.setValue("deck",text);
    deckSet.clear();
    cardSet.clear();
    traverseAnki(model->invisibleRootItem());
    deleteDeck();
    deleteCard();
    rootDeckName=rootDeckNames;
    QMessageBox::information(this, "提示", "同步成功", QMessageBox::Ok);
}


void MainWindow::htmlTemplate()
{

    html->showNormal();

}

void MainWindow::openNote()
{

    if(!settings.value("note").toString().isEmpty()){
        openNoteEditor();
    }else{
        QMessageBox::information(this, "提示", "请先打开Markdown", QMessageBox::Ok);
    }

}

void MainWindow::markdown()
{

    QString fileName = QFileDialog::getOpenFileName(this,"打开md笔记文件(没有就新建txt把扩展改为md)",settings.value("history").toString(),"笔记文件 (*.*)");
    if (!fileName.isEmpty()) {
        QString lastDirectory = QFileInfo(fileName).absolutePath();
        settings.setValue("history",lastDirectory);
        settings.setValue("note",fileName);
        setTreeView(readFile(fileName));
        changPath();

    }
}

void MainWindow::gram()
{

    grammar->showNormal();
}

void MainWindow::qq()
{
    QUrl url("https://ztough.cn/Buy/NoteToCard");
    QDesktopServices::openUrl(url);
}

void MainWindow::jiaochen()
{
    QUrl url("https://tkjpydwgii.feishu.cn/wiki/DAUKwwEPFi5RBWkw5QQcgUW9nj2?from=from_copylink");
    QDesktopServices::openUrl(url);
}

