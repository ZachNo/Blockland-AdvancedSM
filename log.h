#ifndef LOG_H
#define LOG_H

#include <QtWidgets>

namespace Ui
{
    class LogWindow;
}

class LogWindow : public QWidget
{
    Q_OBJECT
public:
    LogWindow(QWidget *parent = 0);
    ~LogWindow();
    QString logText;
private:
    QTextEdit *logBox;
    bool isSliding;
public slots:
    void updateLogForce();
    void startSlider();
    void stopSlider();
};

#endif // LOG_H
