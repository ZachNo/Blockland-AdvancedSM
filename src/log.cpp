#include "log.h"

LogWindow::LogWindow(QWidget *parent) : QWidget(parent)
{
    //Create textbox in window
    this->setFixedSize(400,300);
    this->setWindowTitle(tr("ASM: Log Window"));
    logBox = new QTextEdit;
    logBox->setFixedSize(390,290);
    logBox->setParent(this);
    logBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    logBox->setAcceptDrops(false);
    logBox->setReadOnly(true);
    logBox->move(5,5);


    //How to tell if scroll bar is currently being used
    connect(logBox->verticalScrollBar(), SIGNAL(sliderPressed()), this, SLOT(startSlider()));
    connect(logBox->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(stopSlider()));

    isSliding = false;

    //Start loop for updating log window (should do this differently later)
    this->updateLogForce();
}

LogWindow::~LogWindow()
{
    delete logBox;
}

void LogWindow::updateLogForce()
{
    //Update text and set scrollbar
    double pos = logBox->verticalScrollBar()->value();
    logBox->setText(logText);
    if(!isSliding)
        logBox->verticalScrollBar()->setValue(logBox->verticalScrollBar()->maximum());
    else
        logBox->verticalScrollBar()->setValue(pos);
    //QTimer::singleShot(100, this, SLOT(updateLogForce()));
}

void LogWindow::startSlider()
{
    isSliding = true;
}

void LogWindow::stopSlider()
{
    isSliding = false;
}
