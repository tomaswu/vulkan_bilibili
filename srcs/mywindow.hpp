#pragma once
#include <QWindow>
#include "render.hpp"
#include <QTimer>

class MyWindow : public QWindow{
    Q_OBJECT
public:
    MyWindow(QWindow *parent = nullptr);
    ~MyWindow();

    void closeEvent(QCloseEvent *event) override;

    QTimer timer;
private:
    Render *render_;
};