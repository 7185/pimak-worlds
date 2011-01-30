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
         void writeSettings();
         QString getHost();
         QString getNickname();
         int getPort();

    public slots:
         void applyEvent();

    protected:
         void closeEvent(QCloseEvent *event);

    private:
         QSettings *settings;
         QLineEdit *nickname;
         QLineEdit *host;
         QSpinBox *port;

};

#endif // SETTINGSWINDOW_H
