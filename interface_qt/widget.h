#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
public slots:
    void EventHandler_for_button1();
    void EventHandler_for_button2();
    void EventHandler_for_button3();
    void EventHandler_for_button4();

};

#endif // WIDGET_H
