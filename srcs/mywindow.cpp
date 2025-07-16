#include "mywindow.hpp"

MyWindow::MyWindow(QWindow *parent)
{
    setSurfaceType(QWindow::VulkanSurface);
    resize(800, 600);
}

MyWindow::~MyWindow()
{
}
