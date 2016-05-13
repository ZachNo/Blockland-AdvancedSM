#ifndef ADDBAN
#define ADDBAN

#include <QtWidgets>
#include <QPixmap>

class MainWindow;

class AddbanWindow : public QWidget
{
    Q_OBJECT
public:
    AddbanWindow(MainWindow *m, QWidget *parent = 0);
    ~AddbanWindow();
private:
    QLineEdit *blid;
    QLineEdit *time;
    QLineEdit *reason;
    QPushButton *okay;
	MainWindow *mw;
private slots:
    void callAddBan();
};

#endif // ADDBAN

