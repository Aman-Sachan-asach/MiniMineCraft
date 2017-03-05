#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QImage>
#include <QGraphicsScene>
#include "scene/scene.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void DisplayQImage();


private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
