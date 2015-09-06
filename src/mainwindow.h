#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _CRT_SECURE_NO_WARNINGS

#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>
#include <QPaintEvent>
#include <QTime>

#include <vector>
#include <cv.h>
#include <highgui.h>
#include "setparameterdialog.h"

using std::vector;
using namespace cv;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_setPushButton_clicked();
    void on_openPushButton_clicked();
    void on_autoPushButton_clicked();
    void on_savePushButton_clicked();
    void on_redoPushButton_clicked();
    void on_undoPushButton_clicked();
    void on_contourModelPushButton_clicked();

private:
    Ui::MainWindow *ui;

    SetParameterDialog * prSetDlg;

    QString inputPath;
    QString outputPath;
    QString imgInputPath;
    QString imgOutputPath;

    unsigned int startIdx,frameIdx;

    Mat img_cur,img_pre;

    vector<Point> contour;
    Rect validRegion;

    QFile file,hisfile;

    QDateTime startTime,curTime;
    int timerId;

    bool drawMouseMove;
    bool useHull;

    bool autoModel;
    enum{NOT_SET=0,IN_PROCESS=1,SET=2};
    Mat mask;
    Mat img_cur_copy;
    Mat bgdModel,fgdModel;
    Rect rect;
    uchar rectState,lblsState,prLblsState;
    vector<Point> fgdPxls,bgdPxls,prFgdPxls,prBgdPxls;
    bool isInitialized;
    bool isf;

private:
    void displayMat(Mat mat);
    bool readImage();
    void drawConnects();
    void contourFill();
    void getBinMask(const Mat& comMask, Mat& binMask);
    void resetAMParameters();
    void restrictPoint(Point& _point);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
