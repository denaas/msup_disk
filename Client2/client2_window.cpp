#include "client2_window.h"
#include "ui_client2_window.h"

void Client2_window::EventHandler_for_button1(void)
{
    ui->widget->setVisible(false);
    ui->widget_2->setVisible(true);
}

void Client2_window::EventHandler_for_button2(void)
{
    ui->widget_2->setVisible(false);
    ui->widget->setVisible(true);
}

void Client2_window::EventHandler_for_button3(void)
{
    ui->widget_2->setVisible(false);
    ui->widget->setVisible(true);
}

Client2_window::Client2_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client2_window)
{
    ui->setupUi(this);
    ui->widget_2->setVisible(false);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this , SLOT(EventHandler_for_button1()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this , SLOT(EventHandler_for_button2()));
    QObject::connect(ui->pushButton_3, SIGNAL(clicked()), this , SLOT(EventHandler_for_button3()));
}

Client2_window::~Client2_window()
{
    delete ui;
}
