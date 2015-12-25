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


char * pin = new char[100];


void error_detected(const char * s)
{
    perror(s);
    exit(1);
}

void make_socket(char *str, char * res)//создаем сокет и посылаем серверу три параметра: действие, адрес и пин-код
{
    struct sockaddr_in addr;
    int ls,i;
    int port = 1200;                                        //порт для подключения
    //-------------------работа с сокетом---------
    ls = socket(AF_INET,SOCK_STREAM, 0);
    if (ls == -1)
        error_detected("ls");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (connect(ls, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        error_detected("connect");
    //------------------посылаем данные
    char line[9];
    line[0] = 'd';
    line[1] = 'e';
    line[2] = 'c';
    line[3] = 'o';
    line[4] = 'd';
    line[5] = 'e';
    line[6] = '\n';
    line[7] = '\0';
    write(ls,line,strlen(line)+1);
    write(ls, str, strlen(str)+1);
    //write(ls, log, strlen(log)+1);
    write(ls, pin, strlen(pin)+1);
    //------------------считываем ответ
    i = 0;
    do {
    if (read(ls, res+i, 1) == 0) printf("read error\n");
    }
    while(res[i++] != '\0');
    close(ls);
}

void Client2_window::EventHandler_for_button1(void) //работа кнопки Read
{
    ui->widget->setVisible(false);
    ui->widget_2->setVisible(true);
}

void Client2_window::EventHandler_for_button2(void) //работа кнопки Ok
{
    QString helpp=ui->lineEdit_3->text();           //считываем пинкод из эдита
    QByteArray qp = helpp.toUtf8();
    pin = qp.data();
    QString tfile=ui->lineEdit->text();           //cчитываем адрес файла
    QByteArray qb = tfile.toUtf8();
    char *str = qb.data();
    char *res = new char[10];
    make_socket(str,res); //вызываем функцию работы с сокетом
    if (!strcmp(res,"Okey\n"))    //проверяем результат
    {
        ui->textEdit->setText("");
        FILE *f = fopen("../decoded_file.txt", "r");
        char *str_for_reading = new char[1024];
        while (fgets(str_for_reading, 1024, f) != NULL)
        {
            int len = strlen(str_for_reading);
            if (str_for_reading[len - 1] == '\n')
                str_for_reading[len - 1] = '\0';
            ui->textEdit->append(str_for_reading);
        }
        delete [] str_for_reading;
        fclose(f);
        //unlink(str);
    }
    else
        ui->textEdit->setText("File hasn't been decoded");
    delete [] res;
    ui->widget_2->setVisible(false);
    ui->widget->setVisible(true);
}

void Client2_window::EventHandler_for_button3(void) //работа кнопки cancel
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
