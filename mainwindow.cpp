#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTimer.h"
#include <QFileDialog>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

VideoCapture capture;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText("welcome!");
    image.load("./vedioProcess.jpg");
    ui->label->resize(image.width(),image.height());
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->pushButton_backward->setFixedSize(30,30);
    ui->pushButton_forward->setFixedSize(30,30);
    ui->pushButton_pause->setFixedSize(60,30);
    ui->pushButton_detect->setFixedSize(120,30);
    ui->pushButton_noDetect->setFixedSize(120,30);
    ui->pushButton_backward->setVisible(false);
    ui->pushButton_forward->setVisible(false);
    ui->pushButton_pause->setVisible(false);
    ui->pushButton_detect->setVisible(false);
    ui->pushButton_noDetect->setVisible(false);
    ui->horizontalSlider->setVisible(false);
    ui->horizontalSlider->setMaximum(1000000);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setSingleStep(1);
    ui->now->setVisible(false);
    ui->end->setVisible(false);
    timer = new QTimer(this);
    this->setWindowTitle("Vedio Processor");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage  Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)                             //3 channels color image
    {
        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols, cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }

    return qImg;

}

QString MainWindow::formatTime(int ms)
{
    int ss = 1000;
    int mi = ss * 60;
    int hh = mi * 60;
    int dd = hh * 24;

    long day = ms / dd;
    long hour = (ms - day * dd) / hh;
    long minute = (ms - day * dd - hour * hh) / mi;
    long second = (ms - day * dd - hour * hh - minute * mi) / ss;
    long milliSecond = ms - day * dd - hour * hh - minute * mi - second * ss;

    QString hou = QString::number(hour,10);
    QString min = QString::number(minute,10);
    QString sec = QString::number(second,10);
    QString msec = QString::number(milliSecond,10);
    //qDebug() << "minute:" << min << "second" << sec << "ms" << msec <<endl;
    return hou + ":" + min + ":" + sec ;
}

void MainWindow::nextFrame()
{
    capture >> frame;
    now=capture.get(CV_CAP_PROP_POS_MSEC);
    ui->now->setText(formatTime(now));
    ui->horizontalSlider->setValue(now/end*1000000);
    //ui->now->setText(QString::number(ui->horizontalSlider->value()));
    if (!frame.empty())
    {
        image = Mat2QImage(frame);
        ui->label->setPixmap(QPixmap::fromImage(image));
        //this->update();
    }
}

void MainWindow::detectFrame()
{
    CascadeClassifier faceDetector;
    string faceCascadeFilename = "haarcascade_frontalface_default.xml";

    //友好错误信息提示
    try{
        faceDetector.load(faceCascadeFilename);
    }
    catch (cv::Exception e){}
    if (faceDetector.empty())
    {
        cerr << "脸部检测器不能加载 (";
        cerr << faceCascadeFilename << ")!" << endl;
        exit(1);
    }
    capture >> frame;
    Mat frameBGR;
    cvtColor(frame,frameBGR,CV_RGB2BGR);
    if (!frame.empty())
    {
        Mat gray;
        cvtColor(frame,gray,CV_BGR2GRAY);
        //直方图均匀化(改善图像的对比度和亮度)
        Mat equalizedImg;
        equalizeHist(gray, equalizedImg);
        int flags = CASCADE_FIND_BIGGEST_OBJECT|CASCADE_DO_ROUGH_SEARCH;	//只检测脸最大的人
        //int flags = CASCADE_SCALE_IMAGE;	//检测多个人
        Size minFeatureSize(50, 50);
        float searchScaleFactor = 1.1f;
        int minNeighbors = 4;
        vector<Rect> faces;
        faceDetector.detectMultiScale(equalizedImg, faces, searchScaleFactor, minNeighbors, flags, minFeatureSize);
        //画矩形框
        for (size_t i = 0; i < faces.size(); i++)
        {
            if (faces[i].height > 0 && faces[i].width > 0)
            {
                //cout<<"("<<faces[i].x<<","<<faces[i].y<<")"<<endl;
                rectangle(equalizedImg, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
                rectangle(gray, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
                rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
            }
        }
        //rectangle (frame, Point(10,50), Point(70, 120), Scalar(0, 255, 255), 1, 8, 0);
        image = Mat2QImage(frame);
        ui->label->setPixmap(QPixmap::fromImage(image));
        //this->update();
    }
    //delete faceDetector;
}

void MainWindow::writeNextFrame()
{
    Mat frameBGR;
    cvtColor(frame,frameBGR,CV_RGB2BGR);
    writer.write(frameBGR);
}

void MainWindow::on_pushButton_clicked()
{
    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
        delete timer;
        QString filename =QFileDialog::getOpenFileName(this,tr("Open Video File"),".",tr("Video Files(*.avi *.mp4 *.flv *.mkv)"));
        capture.open(filename.toLocal8Bit().data());
        if (capture.isOpened())
        {
            ui->pushButton_backward->setVisible(true);
            ui->pushButton_forward->setVisible(true);
            ui->pushButton_pause->setVisible(true);
            ui->horizontalSlider->setVisible(true);
            ui->pushButton_detect->setVisible(false);
            ui->pushButton_noDetect->setVisible(false);
            ui->now->setVisible(true);
            ui->end->setVisible(true);
            rate= capture.get(CV_CAP_PROP_FPS);
            fcount=capture.get(CV_CAP_PROP_FRAME_COUNT);
            end=1000/rate*fcount;
            ui->end->setText(formatTime(end));
            ui->now->setText(formatTime(0));
            capture >> frame;
            if (!frame.empty())
            {
                image = Mat2QImage(frame);
                ui->label->resize(image.width(),image.height());
                ui->label->setPixmap(QPixmap::fromImage(image));
                timer = new QTimer(this);
                timer->setInterval(1000/rate);   //set timer match with FPS
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
                timer->start();
            }
        }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (capture.isOpened())
            capture.release();     //decide if capture is already opened; if so,close it
        capture.open(0);           //open the default camera
        //打开摄像头
        delete timer;
        if (capture.isOpened())
        {
            ui->pushButton_backward->setVisible(false);
            ui->pushButton_forward->setVisible(false);
            ui->pushButton_pause->setVisible(false);
            ui->horizontalSlider->setVisible(false);
            ui->now->setVisible(false);
            ui->end->setVisible(false);
            ui->pushButton_detect->setVisible(true);
            ui->pushButton_noDetect->setVisible(true);
            capture >> frame;
            if (!frame.empty())
            {
                rate=24.0;
                image = Mat2QImage(frame);
                ui->label->resize(image.width(),image.height());
                ui->label->setPixmap(QPixmap::fromImage(image));
                timer = new QTimer(this);
                timer->setInterval(1000/rate);   //set timer match with FPS
                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
                timer->start();
            }
        }
}

void MainWindow::on_pushButton_6_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_3_clicked()
{
    cv::Mat cannyImg ;
    cv::Canny(frame, cannyImg, 50, 80, 3);
    cv::namedWindow("Canny");
    cv::imshow("Canny", cannyImg);
}

void MainWindow::on_pushButton_4_clicked()
{
    writer.open("./myrec.avi",CV_FOURCC('M', 'J', 'P', 'G') ,rate, cv::Size(frame.cols, frame.rows),true);
    ui->pushButton_4->setDisabled(true); //if successfully start videoWriter, disable the button
    if (!frame.empty())
    {
        timer = new QTimer(this);
        timer->setInterval(1000/rate);   //set timer match with FPS
        connect(timer, SIGNAL(timeout()), this, SLOT(writeNextFrame()));
        timer->start();
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    writer.release();
    ui->pushButton_4->setDisabled(false);
}

void MainWindow::on_pushButton_pause_clicked()
{
    if (ui->pushButton_pause->text()==QString("pause"))
    {
        timer->stop();
        ui->pushButton_pause->setText("continue");
    }
    else
    {
        timer->start();
        ui->pushButton_pause->setText("pause");
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    double resetTime=position/double(1000000)*end;
    ui->horizontalSlider->setValue(position);
    capture.set(CV_CAP_PROP_POS_MSEC,resetTime);
    ui->now->setText(formatTime(resetTime));
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    timer->start();
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    timer->stop();
}

void MainWindow::on_pushButton_forward_clicked()
{
    double resetTime=capture.get(CV_CAP_PROP_POS_MSEC)+5000;
    capture.set(CV_CAP_PROP_POS_MSEC,resetTime);
    ui->now->setText(formatTime(resetTime));
    ui->horizontalSlider->setValue(resetTime/end*1000000);
}

void MainWindow::on_pushButton_backward_clicked()
{
    double resetTime=capture.get(CV_CAP_PROP_POS_MSEC)-5000;
    capture.set(CV_CAP_PROP_POS_MSEC,resetTime);
    ui->now->setText(formatTime(resetTime));
    ui->horizontalSlider->setValue(resetTime/end*1000000);
}

void MainWindow::on_pushButton_detect_clicked()
{
    delete timer;
    if (capture.isOpened())
    {
        ui->pushButton_backward->setVisible(false);
        ui->pushButton_forward->setVisible(false);
        ui->pushButton_pause->setVisible(false);
        ui->horizontalSlider->setVisible(false);
        ui->now->setVisible(false);
        ui->end->setVisible(false);
        ui->pushButton_detect->setVisible(true);
        ui->pushButton_noDetect->setVisible(true);
        CascadeClassifier faceDetector;
        string faceCascadeFilename = "haarcascade_frontalface_default.xml";

        //友好错误信息提示
        try{
            faceDetector.load(faceCascadeFilename);
        }
        catch (cv::Exception e){}
        if (faceDetector.empty())
        {
            cerr << "脸部检测器不能加载 (";
            cerr << faceCascadeFilename << ")!" << endl;
            exit(1);
        }
        rate= 24.0;
        capture >> frame;
        if (!frame.empty())
        {
            Mat gray;
            cvtColor(frame,gray,CV_BGR2GRAY);
            //直方图均匀化(改善图像的对比度和亮度)
            Mat equalizedImg;
            equalizeHist(gray, equalizedImg);
            int flags = CASCADE_FIND_BIGGEST_OBJECT|CASCADE_DO_ROUGH_SEARCH;	//只检测脸最大的人
            //int flags = CASCADE_SCALE_IMAGE;	//检测多个人
            Size minFeatureSize(30, 30);
            float searchScaleFactor = 1.1f;
            int minNeighbors = 4;
            std::vector<Rect> faces;
            faceDetector.detectMultiScale(equalizedImg, faces, searchScaleFactor, minNeighbors, flags, minFeatureSize);
            //画矩形框
            cv::Point text_lb;
            for (size_t i = 0; i < faces.size(); i++)
            {
                if (faces[i].height > 0 && faces[i].width > 0)
                {
                    text_lb = cv::Point(faces[i].x, faces[i].y);
                    cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 1, 8, 0);
                }
            }
            image = Mat2QImage(frame);
            ui->label->resize(image.width(),image.height());
            ui->label->setPixmap(QPixmap::fromImage(image));
            timer = new QTimer(this);
            timer->setInterval(1000/rate);   //set timer match with FPS
            connect(timer, SIGNAL(timeout()), this, SLOT(detectFrame()));
            timer->start();
        }
    }
    //delete faceDetector;
}

void MainWindow::on_pushButton_noDetect_clicked()
{
    delete timer;
    if (capture.isOpened())
    {
        ui->pushButton_backward->setVisible(false);
        ui->pushButton_forward->setVisible(false);
        ui->pushButton_pause->setVisible(false);
        ui->horizontalSlider->setVisible(false);
        ui->now->setVisible(false);
        ui->end->setVisible(false);
        ui->pushButton_detect->setVisible(true);
        ui->pushButton_noDetect->setVisible(true);
        rate= 24.0;
        capture >> frame;
        if (!frame.empty())
        {
            image = Mat2QImage(frame);
            ui->label->resize(image.width(),image.height());
            ui->label->setPixmap(QPixmap::fromImage(image));
            timer = new QTimer(this);
            timer->setInterval(1000/rate);   //set timer match with FPS
            connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
            timer->start();
        }
    }
}
