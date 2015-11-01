#include "widget.h"
#include "ui_widget.h"

void Widget::EventHandler_for_button1(void)
{
   ui->lineEdit_2->setText("Masterkey was successfully created");
   ui->pushButton->setVisible(false);
   ui->pushButton_2->setVisible(false);
   ui->pushButton_3->setVisible(false);
   ui->pushButton_4->setVisible(false);
   ui->lineEdit->setVisible(false);
   ui->lineEdit_2->setVisible(false);
   ui->widget->setVisible(true);
   //здесь надо написать код для создания мастерключа
   //ui->widget->setVisible(false);
   //ui->pushButton->setVisible(true);
   //ui->pushButton2->setVisible(true);
   //ui->pushButton3->setVisible(true);
   //ui->pushButton4->setVisible(true);
   //ui->lineEdit->setVisible(true);
   //ui->lineEdit_2->setVisible(true);
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
}

Widget::~Widget()
{
    delete ui;
}

