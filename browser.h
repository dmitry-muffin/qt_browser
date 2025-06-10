//
// Created by Дмитрий Балясный  on 06.06.2025.
//

#ifndef BROWSER_H
#define BROWSER_H

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QNetworkAccessManager>
#include <QTextEdit>
#include <QComboBox>
#include <QToolButton>
class Browser : public QWidget {
Q_OBJECT

public:
    explicit Browser(QWidget *parent = nullptr);
    ~Browser() override;

private:
    QLineEdit *urlBar;
    QPushButton *backButton;
    QPushButton *goButton;
    QPushButton *answersButton;
    QComboBox *historyMenu;
    QToolButton *mainMenu;

    QWebEngineView *webView;
    QNetworkAccessManager *networkManager;
    QWebEngineHistory *history;

    QAction *about;
    QAction *historyClr;

    void showAnswerWindow(const QString &answer);
    void sendRequest(const QString &prompt);
    void setupVisual();
    void setupElements();
    void setupConnections();
    void setupMenu();
    void logToFile(const QString &message);

private slots:
    void onSearch();
    void onReturn() const;
    void findAnswers();
    void showHistory();
    void loadFromHistory(int idx);
    void onPageLoadFinished(bool ok);
    void onHistoryClr();
    void onAbout();
};

#endif //BROWSER_H
