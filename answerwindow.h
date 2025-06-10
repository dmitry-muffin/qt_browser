//
// Created by Дмитрий Балясный  on 06.06.2025.
//

#ifndef ANSWERWINDOW_H
#define ANSWERWINDOW_H

#include <QDialog>
#include <QTextEdit>

#include "answerwindow.h"


class answerwindow : public QDialog {
Q_OBJECT

public:
    explicit answerwindow(const QString &text,QWidget *parent = nullptr);
    ~answerwindow() override;

private:
    QTextEdit *textEdit;
};


#endif //ANSWERWINDOW_H
