#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);

    QString locale = QLocale::system().name().split('_')[0];
    QTranslator t;
    t.load(QString("client_") + locale);
    app.installTranslator(&t);

    MainWindow window;
    window.show();
    window.showRenderZone();

    return app.exec();
}
