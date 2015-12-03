#ifndef CLIENT2_WINDOW_H
#define CLIENT2_WINDOW_H

#include <QMainWindow>

namespace Ui {
class Client2_window;
}

class Client2_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client2_window(QWidget *parent = 0);
    ~Client2_window();

private:
    Ui::Client2_window *ui;
public slots:
    void EventHandler_for_button1();
    void EventHandler_for_button2();
    void EventHandler_for_button3();
};

#endif // CLIENT2_WINDOW_H
