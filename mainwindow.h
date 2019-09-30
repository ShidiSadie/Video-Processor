#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "QTimer.h"

#include <QMainWindow>
#include <opencv2/opencv.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString formatTime(int ms);
    ~MainWindow();

private slots:
    void nextFrame();
    void detectFrame();
    void writeNextFrame();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_pause_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_sliderPressed();

    void on_pushButton_forward_clicked();

    void on_pushButton_backward_clicked();

    void on_pushButton_detect_clicked();

    void on_pushButton_noDetect_clicked();

private:
    Ui::MainWindow *ui;
    cv::Mat frame;
    cv::VideoCapture capture;
    QImage  image;
    QTimer *timer;
    double rate; //FPS
    cv::VideoWriter writer;   //make a video record
    double now;     //视频当前进行时间
    double end;     //视频总时间
    int fcount;     //总帧数
};

#endif // MAINWINDOW_H
