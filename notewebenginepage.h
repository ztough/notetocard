#ifndef NOTEWEBENGINEPAGE_H
#define NOTEWEBENGINEPAGE_H

#include <QWebEnginePage>


class NoteWebEnginePage : public QWebEnginePage {
private:
    QList<NoteWebEnginePage*> pages;
public:
    NoteWebEnginePage(QObject *parent = nullptr);
protected:
    QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override {
        cleanupPages();
        NoteWebEnginePage *newPage = new NoteWebEnginePage();
        pages.append(newPage);
        return newPage;
    }
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override {

        return true;
    }
    void cleanupPages() {
        for (NoteWebEnginePage *page : pages) {
            delete page;
        }
        pages.clear();
    }

};
#endif // NOTEWEBENGINEPAGE_H
