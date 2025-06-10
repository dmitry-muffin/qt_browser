#include <QApplication>
#include "browser.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Browser browser;
    browser.show();
    return app.exec();
}
