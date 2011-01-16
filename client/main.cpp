#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
