//
// Created by Дмитрий Балясный  on 06.06.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_answerwindow.h" resolved

#include "answerwindow.h"
#include <QVBoxLayout>

answerwindow::answerwindow(const QString &text,QWidget *parent) :
    QDialog(parent) {
    setWindowTitle("Ответы");
    resize(600, 400);

    auto *layout = new QVBoxLayout(this);
    textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setPlainText(text);

    layout->addWidget(textEdit);
}

answerwindow::~answerwindow() {

}
