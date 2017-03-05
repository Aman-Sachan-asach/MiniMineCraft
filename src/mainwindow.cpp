#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"



#include <QPixmap>
#include <QImageWriter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::DisplayQImage()
{

    QImage image(512, 512, QImage::Format_RGB32);
    QRgb value = qRgb(0, 0, 0);
    image.fill(value);

    //have for loop going through x and y of screen
        //image.setPixel(100, 100, qRgb(122, 163, 39));//x, y, perlin_z value


    QPixmap pixmap(QPixmap::fromImage(image));
    QGraphicsScene graphics_scene;
    graphics_scene.addPixmap(pixmap);
    graphics_scene.setSceneRect(pixmap.rect());
    //ui->scene_display->setScene(&graphics_scene);
}



void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
