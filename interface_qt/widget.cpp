#include "widget.h"
#include "ui_widget.h"
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

#define N_GETUNLIM 5

using std::cout;
using std::endl;
using std::vector;

int buttom = 0;
char * pin = new char[100];

void error_detected(const char * s)
{
    perror(s);
    exit(1);
}

char *getlineunlim()                                 //функция считывает строку до \n
{
    char *p;
    int i = N_GETUNLIM, k = 0;
    if (!(p = (char*) malloc(i * sizeof(char)))) goto fail;
    fgets(&p[k], N_GETUNLIM, stdin);
    k = strlen(p);
    while (p[k-1]!='\n')
    {
        if ((k+N_GETUNLIM)>i)
        {
            i *= 2;
            if (!(p = (char*) realloc(p, i * sizeof(char)))) goto fail;
        }
        fgets(&p[k], N_GETUNLIM, stdin);
        k = strlen(p);
    }
    return p;
    fail:
        p = NULL;
        return p;
}

int count_USB(){                                     //считает количество воткнутых флешек, ее же надо вставить в демон для проверки
    int pid1,pid2, fd1[2],fd2[2],numb = 0;
    pipe(fd1);
    pid1 = fork();
    if(!pid1){
        pipe(fd2);
        pid2 = fork();
        if(!pid2){
            dup2(fd2[1],1);
            close(fd1[1]);
            close(fd1[0]);
            close(fd2[0]);
            close(fd2[1]);
            execlp("blkid","blkid","-t","TYPE=vfat",NULL);
            return -1;
        }
        else {
        dup2(fd1[1],1);
        dup2(fd2[0],0);
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        execlp("wc","wc","-l" ,NULL);
        return -1;
        }
    }
    else {
        wait(NULL);
        dup2(fd1[0],0);
        close(fd1[0]);
        close(fd1[1]);
        char * p;
        p = getlineunlim();
        int i,k;
        k = strlen(p);
        numb = 0;
        for (i = 0; i < k-1; ++i) numb = numb*10+p[i]- '0';
    }
    return numb;
}


void takeusbinf(vector<char*> &data){                 //узнает информацию о воткнутой флешке, записывает ее в вектор data: 0)LABEL 1)UUID
    int pid,fd[2];
    pipe(fd);
    pid = fork();
    if(!pid){
         dup2(fd[1],1);
         close(fd[0]);
         close(fd[1]);
         execlp("blkid","blkid","-t" ,"TYPE=vfat",NULL);
    }
    else {
         dup2(fd[0],0);
         close(fd[0]);
         close(fd[1]);
         char * p,*q;
         int z,t,k,i = 0;
         p = getlineunlim();
         k = strlen(p);
         while(p[i++]!=' ');
         for (z = 0; z < 2; ++z){
            while(p[i++]!='"');
            t = i;
            q = new char[k];
            while(p[i]!='"'){
                q[i-t] = p[i];
                i++;
            }
            q[i-t] = '\0';
            i++;
            data[z] = q;
         }
    }
}


void make_socket(char *line,char *str, char * res)    //создаем сокет и посылаем серверу три параметра: действие, адрес и пин-код
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
    //char *client = "client_1\0";
    //write(ls,client,strlen(client)+1);
    write(ls, line, strlen(line)+1);
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

vector <char*> data_usb(2),check(2);

void Widget::EventHandler_for_button1(void)//кнопка masterkey
{
   buttom = 1;
   int flag;
   flag = count_USB();
   if (flag == 1) {
        takeusbinf(data_usb);
        ui->widget_2->setVisible(false);
        ui->widget->setVisible(true);
   }
   else ui->textEdit_2->setText("Masterkey has not been created,\n because there is not one USB drive inserted");
}

void Widget::EventHandler_for_button2(void) //кнопка encode
{
   buttom = 2;
   ui->widget_2->setVisible(false);
   ui->widget->setVisible(true);
}

void Widget::EventHandler_for_button3(void) //кнопка decode

{
    buttom = 3;
    ui->textEdit_2->setText("Buttom don't work");
   /* ui->widget_2->setVisible(false);
    ui->widget->setVisible(true);*/
}

void Widget::EventHandler_for_button4(void) //кнопка delete
{
    buttom = 4;
    ui->widget_2->setVisible(false);
    ui->widget->setVisible(true);
}

void Widget::EventHandler_for_button5(void) //кнопка Ok
{
    if (buttom == 1) {
             QString helpp=ui->lineEdit_4->text();           //считываем пинкод из эдита
             QByteArray qp = helpp.toUtf8();
             pin = qp.data();
             char *line = "key\n\0";
             char *str = "\n\0";
             char *res = new char[10];
             make_socket(line,str,res);
             //makemasterkey(data_usb,log,pin);
             if (!strcmp(res,"Okey\n")) ui->textEdit_2->setText("Masterkey was successfully created");
             else  ui->textEdit_2->setText("Masterkey has not been created");
             delete []res;
             ui->widget->setVisible(false);
             ui->widget_2->setVisible(true);
        /*else {
            if (flag == 0) ui->textEdit_2->setText("Masterkey has not been created,\n because there is no USB drive inserted");
            else ui->textEdit_2->setText("Masterkey has not been created,\n because there is more than one USB drive inserted");
            ui->widget->setVisible(false);
            ui->widget_2->setVisible(true);
        }*/
   }
   else if (buttom == 2){
        /*QString helpl=ui->lineEdit_3->text();           //считываем логин из первого эдита
        QByteArray ql = helpl.toUtf8();
        log = ql.data();*/
        QString helpp=ui->lineEdit_4->text();           //считываем пинкод из второго эдита
        QByteArray qp = helpp.toUtf8();
        pin = qp.data();
        char *line = "encode\n\0";                        //ставим какая команда
        QString tfile=ui->textEdit->toPlainText();      //читываем адрес шифруемого диска
        QByteArray qb = tfile.toUtf8();
        char *str = qb.data();
        char *res = new char[10];
        make_socket(line,str,res);
        if (!strcmp(res,"Okey\n")) ui->textEdit_2->setText("File was successfully encoded");
        else ui->textEdit_2->setText("File wasnot encoded");
        delete []res;
        ui->widget->setVisible(false);
        ui->widget_2->setVisible(true);
   }
    else if (buttom == 3){
      /*   QString helpl=ui->lineEdit_3->text();          //считываем логин из первого эдита
         QByteArray ql = helpl.toUtf8();
         log = ql.data();
         QString helpp=ui->lineEdit_4->text();           //считываем пинкод из второго эдита
         QByteArray qp = helpp.toUtf8();
         pin = qp.data();
         char *line = "decode\0";                        //ставим какая команда
         QString tfile=ui->textEdit->toPlainText();      //читываем адрес шифруемого диска
         QByteArray qb = tfile.toUtf8();
         char *str = qb.data();
         char *res = new char[5];
         make_socket(line,str,res);
         if (!strcmp(res,"Okey\n")) ui->textEdit_2->setText("File was successfully decoded");
         else ui->textEdit_2->setText("File wasnot decoded");
         delete [] res;
*/
         ui->widget->setVisible(false);
         ui->widget_2->setVisible(true);
    }
    else if (buttom == 4){
         /*QString helpl=ui->lineEdit_3->text();          //считываем логин из первого эдита
         QByteArray ql = helpl.toUtf8();
         log = ql.data();*/
         QString helpp=ui->lineEdit_4->text();           //считываем пинкод из второго эдита
         QByteArray qp = helpp.toUtf8();
         pin = qp.data();
         char *line = "delete\n\0";                        //ставим какая команда
         QString tfile=ui->textEdit->toPlainText();      //читываем адрес шифруемого диска
         QByteArray qb = tfile.toUtf8();
         char *str = qb.data();
         char *res = new char[10];
         make_socket(line,str,res);
         if (!strcmp(res,"Okey\n")) ui->textEdit_2->setText("File was successfully deleted");
         else ui->textEdit_2->setText("File wasnot deleted");
         delete []res;
         ui->widget->setVisible(false);
         ui->widget_2->setVisible(true);
    }

}

void Widget::EventHandler_for_button6(void) //кнопка Cancel
{
    if (buttom == 1)ui->textEdit_2->setText("Masterkey has not been created");
    else if (buttom == 2) ui->textEdit_2->setText("File wasnot encoded");
    else if (buttom == 3) ui->textEdit_2->setText("File wasnot decoded");
    else ui->textEdit_2->setText("File wasnot deleted");
    ui->widget->setVisible(false);
    ui->widget_2->setVisible(true);
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

