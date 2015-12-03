#include "client2_window.h"
#include "ui_client2_window.h"
#include <QString>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <vector>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>


char * log = new char[100];
char * pin = new char[100];


void error_detected(const char * s)
{
    perror(s);
    exit(1);
}

void make_socket(char *str, char * res)
{
    struct sockaddr_in addr;
    int ls,i;
    int port = 1200;
    ls = socket(AF_INET,SOCK_STREAM, 0);
    if (ls == -1)
        error_detected("ls");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (connect(ls, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        error_detected("connect");
    char *client = "client_2\0";
    write(ls,client,strlen(client)+1);
    write(ls, str, strlen(str)+1);
    write(ls, log, strlen(log)+1);
    write(ls, pin, strlen(pin)+1);
    i = 0;
    do {
    if (read(ls, res+i, 1) == 0) printf("read error\n");
    }
    while(res[i++] != '\0');
    close(ls);
}

void Client2_window::EventHandler_for_button1(void)
{
    ui->widget->setVisible(false);
    ui->widget_2->setVisible(true);
}

void Client2_window::EventHandler_for_button2(void)
{
    QString helpl=ui->lineEdit_2->text();          //считываем логин из первого эдита
    QByteArray ql = helpl.toUtf8();
    log = ql.data();
    QString helpp=ui->lineEdit_3->text();           //считываем пинкод из второго эдита
    QByteArray qp = helpp.toUtf8();
    pin = qp.data();
    QString tfile=ui->lineEdit->text();           //читываем адрес файла
    QByteArray qb = tfile.toUtf8();
    char *str = qb.data();
    char *res = new char[5];
    make_socket(str,res);
    if (!strcmp(res,"Okey\0")) ui->textEdit->setText("File was successfully decoded");
    else ui->textEdit->setText("File wasnot decoded");
    delete [] res;
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
