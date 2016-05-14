#include "addban.h"
#include "mainwindow.h"

AddbanWindow::AddbanWindow(MainWindow *m, QWidget *parent) : mw(m), QWidget(parent)
{
    //Create textbox in window
    this->setFixedSize(250,85);
    this->setWindowTitle(tr("Add Ban"));

    blid = new QLineEdit;
    blid->setParent(this);
    blid->move(5,5);
    blid->setFixedWidth(75);
    blid->setPlaceholderText("BLID");

    time = new QLineEdit;
    time->setParent(this);
    time->move(85,5);
    time->setFixedWidth(160);
    time->setPlaceholderText("Time (in minutes)");

    reason = new QLineEdit;
    reason->setParent(this);
    reason->move(5,30);
    reason->setFixedWidth(240);
    reason->setPlaceholderText("Reason");

    okay = new QPushButton;
    okay->setParent(this);
    okay->move(5,55);
    okay->setFixedWidth(240);
    okay->setText("Ban");

    connect(okay, SIGNAL(clicked(bool)), this, SLOT(callAddBan()));
}

AddbanWindow::~AddbanWindow()
{
    delete blid;
    delete time;
    delete reason;
    delete okay;
}

void AddbanWindow::callAddBan()
{
    this->hide();
    if(blid->text() == tr("") || time->text() == tr("") || reason->text() == tr(""))
        return;

    mw->addRealBan(blid->text(), time->text(), reason->text());
}
