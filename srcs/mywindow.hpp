#pragma once
#include <QWindow>

class MyWindow : public QWindow{
    Q_OBJECT
public:
    MyWindow(QWindow *parent = nullptr);
    ~MyWindow();
};