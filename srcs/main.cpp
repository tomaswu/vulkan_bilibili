#include <iostream>
#include "render.hpp"
#include <QApplication>
#include "mywindow.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Hello, world!" << std::endl;
    auto app = QApplication(argc, argv);
    auto window = MyWindow();
    window.show();
    Render render(&window);
    return app.exec();
}
