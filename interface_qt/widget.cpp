#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
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

void makemasterkey(vector<char*> &data,QString log,QString pin){};


vector <char*> data_usb(2),check(2);
void Widget::EventHandler_for_button1(void)
{
   int flag;
   flag = count_USB();
   if (flag == 1) {
        takeusbinf(data_usb);
        ui->pushButton->setVisible(false);
        ui->pushButton_2->setVisible(false);
        ui->pushButton_3->setVisible(false);
        ui->pushButton_4->setVisible(false);
        ui->lineEdit->setVisible(false);
        ui->lineEdit_2->setVisible(false);
        ui->widget->setVisible(true);
   }
   else ui->lineEdit_2->setText("Masterkey has not been created,\r\n because there is not one USB drive inserted");
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
    flag = count_USB();
    if (flag == 1) {
        takeusbinf(check);
        if (!strcmp(data_usb[0],check[0]) && !strcmp(data_usb[1],check[1]))  {
            /*cout<<data_usb[0]<<endl;
            cout<<data_usb[1]<<endl;
            cout<<check[0]<<endl;
            cout<<check[1]<<endl;*/
            log = ui->lineEdit_3->text();
            pin = ui->lineEdit_4->text();
            makemasterkey(data_usb,log,pin);
            ui->lineEdit_2->setText("Masterkey was successfully created");
        }
        else ui->lineEdit_2->setText("Masterkey has not been created, because you changed USB");
        ui->widget->setVisible(false);
        ui->pushButton->setVisible(true);
        ui->pushButton_2->setVisible(true);
        ui->pushButton_3->setVisible(true);
        ui->pushButton_4->setVisible(true);
        ui->lineEdit->setVisible(true);
        ui->lineEdit_2->setVisible(true);
    }
    else {
        if (flag == 0) ui->lineEdit_2->setText("Masterkey has not been created,\n because there is no USB drive inserted");
        else ui->lineEdit_2->setText("Masterkey has not been created,\n because there is more than one USB drive inserted");
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

