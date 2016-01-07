#ifndef ABOUT
#define ABOUT

#include <QtWidgets>
#include <QPixmap>

class AboutWindow : public QWidget
{
    Q_OBJECT
public:
    AboutWindow(QWidget *parent = 0);
    ~AboutWindow();
private:
    QLabel *aboutImage;
    QLabel *aboutBox;
    QPixmap *image;
};

#endif // ABOUT

