#include <QCoreApplication>
#include <iostream>
#include "Server.h"

int main(int argc, char* argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QCoreApplication app(argc, argv);

    if (argc<2) {
        std::cout << "usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    Server server(atoi(argv[1]));

    return app.exec();
}
