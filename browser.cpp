
#include "browser.h"
#include "answerwindow.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QByteArray>
#include <QPointer>
#include <QComboBox>
#include <QWebEngineHistoryItem>
#include <QMenu>
#include "QMessageBox"
#include "QFile"

Browser::Browser(QWidget *parent) :
    QWidget(parent) {

    setupVisual();
    setupElements();
    setupMenu();
    setupConnections();
}

void Browser::setupVisual() {
    setWindowTitle("Sosal Browser");
    this->resize(1024, 768);

}
void Browser::setupElements() {
    auto *layout = new QVBoxLayout(this);
    auto *topBar = new QHBoxLayout();

    networkManager = new QNetworkAccessManager(this);

    urlBar = new QLineEdit(this);
    goButton = new QPushButton(this);
    backButton = new QPushButton(this);
    webView = new QWebEngineView(this);
    answersButton = new QPushButton(this);
    historyMenu = new QComboBox(this);
    mainMenu = new QToolButton(this);
    mainMenu->setIconSize(QSize(60, 32));
    goButton->setText("Search");
    backButton->setText("Return");
    answersButton->setText("Answers");

    topBar->addWidget(goButton, 1);
    topBar->addWidget(urlBar,5);
    topBar->addWidget(backButton,1);
    topBar->addWidget(answersButton,1);
    topBar->addWidget(historyMenu,1);
    topBar->addWidget(mainMenu,1);

    layout->addLayout(topBar);
    layout->addWidget(webView);
}

void Browser::setupConnections() {
    connect(goButton, &QPushButton::clicked, this, &Browser::onSearch);
    connect(backButton, &QPushButton::clicked, this, &Browser::onReturn);
    connect(urlBar, &QLineEdit::returnPressed, this, &Browser::onSearch);
    connect(answersButton, &QPushButton::clicked, this, &Browser::findAnswers);
    connect(historyMenu, QOverload<int>::of(&QComboBox::activated), this, &Browser::loadFromHistory);
    connect(webView, &QWebEngineView::loadFinished,this, &Browser::onPageLoadFinished);
    connect(about, &QAction::triggered, this, &Browser::onAbout);
    connect(historyClr, &QAction::triggered, this, &Browser::onHistoryClr);
    webView->setUrl(QUrl( "https://google.com/"));
}

void Browser::setupMenu() {
    about = new QAction("about", this);
    historyClr = new QAction("Clear history", this);

    auto *menu = new QMenu(this);
    menu->addAction(historyClr);
    menu->addSeparator();
    menu->addAction(about);

    mainMenu->setText("Menu");

    mainMenu->setMenu(menu);
    mainMenu->setPopupMode(QToolButton::InstantPopup);
}

void Browser::logToFile(const QString &message) {
    QFile file("logs.txt");

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString(("yyyy-MM-dd hh:mm:ss")) + " " + message + "\n";
        file.close();
    }

}

void Browser::onAbout() {
    QMessageBox box;
    box.setWindowTitle("About");
    box.setText("Sosal Browser\nВерсия 14.88");

    QPixmap goidaPix("/Users/dmitry_muffin/CLionProjects/untitled/pics/goida_logo_2.webp");

    box.setIconPixmap(goidaPix);
    box.exec();
}

void Browser::onSearch() {
    QString url = urlBar->text();
    if (!url.isEmpty()) {
        if (!url.startsWith("https")) {
            url = "https://" + url;
        }
        webView->load(QUrl(url));
    }
    showHistory();
}

void Browser::onReturn() const {
    webView->back();
}
void Browser::onHistoryClr() {
    history->clear();
    historyMenu->blockSignals(true);
    historyMenu->clear();
    historyMenu->blockSignals(false);
}
void Browser::findAnswers() {
    QPointer<Browser> safeThis(this);

    webView->page()->toPlainText([this](const QString &text) {
        QString prompt = "Найди ответы на вопросы на этой странице:\n" + text;
        logToFile("Отправлен запрос: " +  prompt);
        sendRequest(prompt);
    });
}

void Browser::showHistory() {
    history = webView->history();

    const QList<QWebEngineHistoryItem> items = history->items();

    historyMenu->blockSignals(true);
    historyMenu->clear();

    for (const QWebEngineHistoryItem &item : items) {
        QString title = item.title();
        QString url = item.url().toString();
        if (title.isEmpty()) {
            title = url;
        }
        historyMenu->addItem(title, url);
    }
    historyMenu->blockSignals(false);
}

void Browser::onPageLoadFinished(bool ok) {
    if (ok) {
        showHistory();
        urlBar->setText(webView->url().toString());
        logToFile("Загружена страница: " + webView->url().toString());
    }

}

void Browser::loadFromHistory(const int idx) {
    QString url = historyMenu->itemData(idx).toString();
    if (!url.isEmpty()) {
        webView->load(QUrl(url));
    }
}

void Browser::sendRequest(const QString &prompt) {
    QNetworkRequest request(QUrl("https://llm.api.cloud.yandex.net/foundationModels/v1/completion"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    request.setRawHeader("Authorization", "Api-Key AQVNy1LmL1o0fMUJJEGTbiUuxBbU2RBHyU-3k96s");

    QJsonObject completionOptions;
    completionOptions["stream"] = false;
    completionOptions["temperature"] = 0.5;
    completionOptions["maxTokens"] = 5000;


    QJsonArray messages;
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["text"] = prompt;
    messages.append(userMsg);

    QJsonObject json;
    json["modelUri"] = "gpt://b1gij2uoru9ptl5ej5ek/yandexgpt/latest";
    json["completionOptions"] = completionOptions;
    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray responseData = reply->readAll();
        qDebug() << "Response data:" << responseData;

        QString answer;

        if (reply->error() == QNetworkReply::NoError) {
            QJsonParseError parseError;
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);
            if (parseError.error != QJsonParseError::NoError) {
                answer = "Ошибка разбора JSON: " + parseError.errorString();
            } else {
                QJsonObject obj = jsonResponse.object();
                QJsonObject result = obj["result"].toObject();
                QJsonArray alternatives = result["alternatives"].toArray();

                if (!alternatives.isEmpty()) {
                    QJsonObject firstAlt = alternatives[0].toObject();
                    QJsonObject message = firstAlt["message"].toObject();
                    answer = message["text"].toString();
                } else {
                    answer = "Ответ пустой.";
                }
            }
        } else {
            answer = "Ошибка запроса: " + reply->errorString();
            logToFile("Ошибка запроса: " + reply->errorString());
        }
        logToFile("Получен ответ: " + answer);
        showAnswerWindow(answer);
        reply->deleteLater();
    });
}




void Browser::showAnswerWindow(const QString &text) {
    QMessageBox::about(this, "Answer", text);
}

Browser::~Browser() = default;
