#include "mainwindow.h"

#include <iostream>
#include "terrain/PerlinNoiseGenerator.h"
#include "terrain/TerrainGenerator.h"
#include  "geometry/Face.h"




#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // RenderWidget widget;
    // widget.resize(800, 600);
    // widget.show();

    MainWindow w;
    w.show();

    return app.exec();
}
