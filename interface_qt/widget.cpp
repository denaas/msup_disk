#include "widget.h"
#include "ui_widget.h"
#include <QString>

void takeusbinf(){};
void makemasterkey(QString log,QString pin){};

void Widget::EventHandler_for_button1(void)
{
   ui->pushButton->setVisible(false);
   ui->pushButton_2->setVisible(false);
   ui->pushButton_3->setVisible(false);
   ui->pushButton_4->setVisible(false);
   ui->lineEdit->setVisible(false);
   ui->lineEdit_2->setVisible(false);
   ui->widget->setVisible(true);
}

void Widget::EventHandler_for_button2(void)
{
   ui->lineEdit_2->setText("File was successfully encoded");
}

void Widget::EventHandler_for_button3(void)
{
   ui->lineEdit_2->setText("File was successfully decoded");
}

void Widget::EventHandler_for_button4(void)
{
   ui->lineEdit_2->setText("File was successfully deleted");
}

void Widget::EventHandler_for_button5(void)
{
    //здесь код для создания мастерключа, работает на кнопку "ок"
    int flag = 0;
    QString  log,pin ;
    if (flag == 0) {
        takeusbinf();
        log = ui->lineEdit_3->text();
        pin = ui->lineEdit_4->text();
        makemasterkey(log,pin);
        ui->lineEdit_2->setText("Masterkey was successfully created");
        ui->widget->setVisible(false);
        ui->pushButton->setVisible(true);
        ui->pushButton_2->setVisible(true);
        ui->pushButton_3->setVisible(true);
        ui->pushButton_4->setVisible(true);
        ui->lineEdit->setVisible(true);
        ui->lineEdit_2->setVisible(true);
    }
    else {
        ui->lineEdit_2->setText("Masterkey has not been created");
        ui->widget->setVisible(false);
        ui->pushButton->setVisible(true);
        ui->pushButton_2->setVisible(true);
        ui->pushButton_3->setVisible(true);
        ui->pushButton_4->setVisible(true);
        ui->lineEdit->setVisible(true);
        ui->lineEdit_2->setVisible(true);
    }
}

void Widget::EventHandler_for_button6(void)
{
    ui->lineEdit_2->setText("Masterkey has not been created");
    ui->widget->setVisible(false);
    ui->pushButton->setVisible(true);
    ui->pushButton_2->setVisible(true);
    ui->pushButton_3->setVisible(true);
    ui->pushButton_4->setVisible(true);
    ui->lineEdit->setVisible(true);
    ui->lineEdit_2->setVisible(true);
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->widget->setVisible(false);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this , SLOT(EventHandler_for_button1()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this , SLOT(EventHandler_for_button2()));
    QObject::connect(ui->pushButton_3, SIGNAL(clicked()), this , SLOT(EventHandler_for_button3()));
    QObject::connect(ui->pushButton_4, SIGNAL(clicked()), this , SLOT(EventHandler_for_button4()));
    QObject::connect(ui->pushButton_5, SIGNAL(clicked()), this , SLOT(EventHandler_for_button5()));
    QObject::connect(ui->pushButton_6, SIGNAL(clicked()), this , SLOT(EventHandler_for_button6()));
}

Widget::~Widget()
{
    delete ui;
}

