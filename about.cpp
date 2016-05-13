#include "about.h"

AboutWindow::AboutWindow(QWidget *parent) : QWidget(parent)
{
    //Create textbox in window
    this->setFixedSize(250,140);
    this->setWindowTitle(tr("ASM: About Window"));
    aboutBox = new QLabel;
    aboutBox->setParent(this);
    aboutBox->move(0,60);
    aboutBox->setFixedWidth(250);
    aboutBox->setAlignment(Qt::AlignCenter);
    aboutBox->setText("Created by ZSNO\nSilk Icon Pack made by Mark James\nUses zlib + QuaZip\nzlib uses the zlib license.\nQuaZip uses LGPL 2 or later\nThis application uses LGPL 3");
    image = new QPixmap;
    image->load(":/Logo.png");
    aboutImage = new QLabel;
    aboutImage->setParent(this);
    aboutImage->setFixedWidth(250);
    aboutImage->move(0,5);
    aboutImage->setAlignment(Qt::AlignCenter);
    aboutImage->setPixmap(*image);
}

AboutWindow::~AboutWindow()
{
    delete aboutImage;
    delete aboutBox;
    delete image;
}
