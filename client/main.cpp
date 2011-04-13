#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name().split('_')[0];

    QTranslator d;
    d.load(QString("qt_") + locale);
    QTranslator t;
    t.load(QString("pwclient_") + locale);

    QApplication a(argc, argv);
    a.installTranslator(&d);
    a.installTranslator(&t);

    MainWindow w;
    w.show();
    w.showRenderZone();

    return a.exec();
}
