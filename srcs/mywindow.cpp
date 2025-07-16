#include "mywindow.hpp"

MyWindow::MyWindow(QWindow *parent)
{
    setSurfaceType(QWindow::VulkanSurface);
    resize(800, 600);
    show();
    render_ = new Render(this);
    connect(&timer, &QTimer::timeout, this, [this]() {
        render_->render();
    });
    timer.start(1);
}

MyWindow::~MyWindow()
{

}

void MyWindow::closeEvent(QCloseEvent *event)
{
    timer.stop();
    this->setVulkanInstance(nullptr);
    delete render_;
}
