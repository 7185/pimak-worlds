#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtGui>
#include <QWidget>

class SettingsWindow : public QWidget
{
    Q_OBJECT

    public:
         explicit SettingsWindow(QWidget *parent = 0);
         void readSettings();
         QString getHost();
         QString getNickname();
         int getPort();

    private:


    private:
         QLineEdit *nickname;
         QLineEdit *host;
         QSpinBox *port;

};

#endif // SETTINGSWINDOW_H
