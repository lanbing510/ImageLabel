#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label->setScaledContents(true);

    ui->openPushButton->setEnabled(false);
    ui->autoPushButton->setEnabled(false);
    ui->savePushButton->setEnabled(false);
    ui->redoPushButton->setEnabled(false);
    ui->undoPushButton->setEnabled(false);
    ui->contourModelPushButton->setEnabled(false);


    ui->savePushButton->setShortcut(tr("ctrl+s"));
    ui->redoPushButton->setShortcut(tr("ctrl+r"));
    ui->undoPushButton->setShortcut(tr("ctrl+u"));

    validRegion.x=ui->label->frameGeometry().x();
    validRegion.y=ui->label->frameGeometry().y()+10;
    validRegion.height=ui->label->frameGeometry().height();
    validRegion.width=ui->label->frameGeometry().width();


    prSetDlg=new SetParameterDialog();

    file.setFileName("set.ini");
    if(!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::information(NULL, "File Read Error", file.errorString(),QMessageBox::Ok , QMessageBox::Ok);
    }

    while(!file.atEnd())
    {
        QByteArray line=file.readLine();
        QString strTmp(line);
        strTmp.trimmed();
        if(strTmp.startsWith("SequencePath="))
        {
            prSetDlg->setSeqPath(strTmp.mid(13).trimmed());
        }

        if(strTmp.startsWith("FileFormat="))
        {
           prSetDlg->setFileFormat(strTmp.mid(11).trimmed());
        }

        if(strTmp.startsWith("OutputPath="))
        {
            prSetDlg->setOutputPath(strTmp.mid(11).trimmed());
        }

        if(strTmp.startsWith("StartFrame="))
        {
            prSetDlg->setStartFrame(strTmp.mid(11).trimmed());
        }
    }

    hisfile.setFileName("history.ini");
    if(!hisfile.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        QMessageBox::information(NULL, "File Read Error", file.errorString(),QMessageBox::Ok , QMessageBox::Ok);
    }

    inputPath="";
    outputPath="";
    imgInputPath="";
    imgOutputPath="";

    frameIdx=0;

    drawMouseMove=false;

    useHull=true; //Hull Model When Start

    autoModel=true; //Auto Model When Start

    rectState=NOT_SET;
    lblsState=NOT_SET;
    prLblsState=NOT_SET;
    isInitialized=false;
    isf=true;

}

MainWindow::~MainWindow()
{

    QTextStream out(&file);
    QString strTmp;
    strTmp=QString("%1%2").arg("StartFrame=").arg(prSetDlg->getStartFrame());
    out<<strTmp<<endl;
    file.close();

    QTextStream hisout(&hisfile);
    qint64 elapsedSec=startTime.msecsTo(curTime)/1000;
    strTmp=QString("%1  %2  %3:%4:%5").arg(frameIdx).arg(frameIdx-startIdx).arg(elapsedSec/3600).arg((elapsedSec%3600)/60).arg(elapsedSec%60);
    hisout<<strTmp<<endl;
    hisfile.close();

    delete ui;
}


void MainWindow::displayMat(Mat mat)
{
    Mat rgb;
    QImage img;
    if(mat.channels()==3)
    {
        cvtColor(mat,rgb,CV_BGR2RGB);
        img = QImage((const uchar*)(rgb.data),rgb.cols,rgb.rows,rgb.cols*rgb.channels(),QImage::Format_RGB888);
    }
    else
    {
        img = QImage((const uchar*)(mat.data),mat.cols,mat.rows,mat.cols*mat.channels(),QImage::Format_Indexed8);
    }
    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(ui->label->pixmap()->size());
    ui->label->show();
}



void MainWindow::on_setPushButton_clicked()
{
    ui->openPushButton->setEnabled(false);
    ui->autoPushButton->setEnabled(false);
    ui->savePushButton->setEnabled(false);
    ui->redoPushButton->setEnabled(false);
    ui->undoPushButton->setEnabled(false);
    ui->contourModelPushButton->setEnabled(false);


    if(prSetDlg->exec()==QDialog::Accepted)
    {
       QString seqPath=prSetDlg->getSeqPath();
       QString fileFormat=prSetDlg->getFileFormat();
       QString oPath=prSetDlg->getOutputPath();
       QString startFrame=prSetDlg->getStartFrame();


       file.resize(0);
       QTextStream out(&file);
       QString strTmp;
       strTmp=QString("%1%2").arg("SequencePath=").arg(seqPath);
       out<<strTmp<<endl;
       strTmp=QString("%1%2").arg("FileFormat=").arg(fileFormat);
       out<<strTmp<<endl;
       strTmp=QString("%1%2").arg("OutputPath=").arg(oPath);
       out<<strTmp<<endl;


       if(seqPath==""||fileFormat==""||oPath==""||startFrame=="")
       {
           QMessageBox::information(NULL, "Information", "Please Set The Parematers",QMessageBox::Ok , QMessageBox::Ok);
       }
       else
       {
           if(seqPath.endsWith("\\"))
           {
               inputPath=seqPath+fileFormat;
           }
           else
           {
               inputPath=seqPath+"\\"+fileFormat;
           }

           if(outputPath.endsWith("\\"))
           {
               outputPath=oPath+fileFormat;
           }
           else
           {
               //outputPath=oPath+"\\"+fileFormat;
               outputPath=oPath+"\\frame%.5d.bmp";
           }

           frameIdx=startFrame.toInt();

           ui->openPushButton->setEnabled(true);
       }
    }
    else
    {
       QMessageBox::information(NULL, "Information", "Please Set The Parematers",QMessageBox::Ok , QMessageBox::Ok);
    }

}

void MainWindow::on_openPushButton_clicked()
{
    readImage();
    ui->openPushButton->setEnabled(false);
    ui->autoPushButton->setEnabled(true);
    ui->savePushButton->setEnabled(true);
    ui->redoPushButton->setEnabled(true);
    ui->undoPushButton->setEnabled(true);
    ui->contourModelPushButton->setEnabled(true);

    startIdx=prSetDlg->getStartFrame().toInt();
    startTime=QDateTime::currentDateTime();
    timerId=startTimer(1000);
}

void MainWindow::on_autoPushButton_clicked()
{
    if(autoModel)
    {
        autoModel=false;
        ui->autoPushButton->setText("Auto");
    }
    else
    {
        resetAMParameters();
        autoModel=true;
        ui->autoPushButton->setText("Manual");
    }
}

void MainWindow::on_savePushButton_clicked()
{
    cv::resize(img_cur,img_cur,Size(640,480));

    imgOutputPath.sprintf(outputPath.toStdString().c_str(),frameIdx);
    if(!imwrite(imgOutputPath.toStdString(),img_cur))
    {
        QMessageBox::information(NULL,"Save Error","Please Check Your Save Path!",QMessageBox::Ok);
    }
    else
    {
        frameIdx++;
        readImage();
        if(!readImage())
        {
            frameIdx--;
        }
    }
}

void MainWindow::on_redoPushButton_clicked()
{
    readImage();
}

void MainWindow::on_undoPushButton_clicked()
{
    img_pre.copyTo(img_cur);
    img_pre.copyTo(img_cur_copy);
    displayMat(img_cur);
    contour.clear();
    resetAMParameters();
}

void MainWindow::on_contourModelPushButton_clicked()
{
    if(useHull)
    {
        useHull=false;
        ui->contourModelPushButton->setText("Hull");
    }
    else
    {
        useHull=true;
        ui->contourModelPushButton->setText("Poly");
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)              //{Left Button Be Pressed}
    {
        Point pointTmp(event->x(),event->y());
        restrictPoint(pointTmp);

        if(autoModel)                      //[Auto Model]
        {
            if(rectState==NOT_SET)                                                          // RectMask
            {
                rect.x=pointTmp.x;
                rect.y=pointTmp.y;
                rectState=IN_PROCESS;
            }
            else if(rectState==SET&&QApplication::keyboardModifiers()==Qt::ShiftModifier)   // Shift+LeftClick: GC_FG
            {
                isf=true;
                lblsState=IN_PROCESS;
            }
            else if(rectState==SET&&QApplication::keyboardModifiers()==Qt::ControlModifier) // Ctrl+LeftClick: GC_BG
            {
                isf=false;
                lblsState=IN_PROCESS;
            }
            else if(rectState==SET&&QApplication::keyboardModifiers()==Qt::AltModifier)     // Alt+LeftClick: GC_PR_FG
            {
                isf=true;
                prLblsState=IN_PROCESS;
            }
            else if(rectState==SET)                                                         // LeftClick: GC_PR_BG
            {
                isf=false;
                prLblsState=IN_PROCESS;
            }

        }
        else                               //[Manual Model]
        {
            contour.push_back(pointTmp);
            drawConnects();
        }

        drawMouseMove=true;
    }
    else if(event->button()==Qt::RightButton)        //{Right Button Be Pressed}
    {
        if(autoModel)                      //[Auto Model]
        {
            //Ctrl+RightClick One: Draw The Contour; Double: Fill The Contour
            if(rectState!=IN_PROCESS&&lblsState!=IN_PROCESS&&prLblsState!=IN_PROCESS)
            {
                if(QApplication::keyboardModifiers()==Qt::ControlModifier)
                {
                    static int rightClickCount=0;
                    if(rightClickCount==0)
                    {
                        Mat binMask;
                        getBinMask(mask, binMask);
                        vector<vector<Point>> contours;
                        findContours(binMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

                        if(contours.size()==1)
                        {
                            contour=contours[0];
                        }
                        else
                        {
                            QMessageBox::information(NULL,"Information","Please Label One Foreground Object !",QMessageBox::Ok);
                        }

                        drawConnects();

                        resetAMParameters();
                        rightClickCount=1;
                    }
                    else if(rightClickCount==1)
                    {
                        contourFill();
                        rightClickCount=0;
                    }
                }
                else    //RightClick: Cut The Foreground Iteratively
                {
                    if (isInitialized)
                        grabCut(img_cur, mask, rect, bgdModel, fgdModel, 1);
                    else
                    {
                        if (lblsState == SET || prLblsState == SET)
                            grabCut(img_cur, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
                        else
                            grabCut(img_cur, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);

                        isInitialized = true;
                    }
                    bgdPxls.clear(); fgdPxls.clear();
                    prBgdPxls.clear(); prFgdPxls.clear();


                    Mat binMask;
                    Mat res;

                    if (!isInitialized)
                        img_cur.copyTo(img_cur_copy);
                    else
                    {
                        img_cur_copy.setTo(Scalar::all(0));
                        getBinMask(mask, binMask);
                        img_cur.copyTo(img_cur_copy, binMask);
                    }
                    displayMat(img_cur_copy);
                }
            }
            else
            {
                resetAMParameters();
            }
        }
        else                               //[Manual Model]
        {
            contourFill();
        }
    }
    else if(event->button()==Qt::MiddleButton)       //{Middle button be pressed}
    {
        if(!contour.empty())
        {
            contour.pop_back();
            drawConnects();
        }
        else
        {
            QMessageBox::information(NULL,"Information","Points Pool Is Empty !",QMessageBox::Ok);
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        Point pointTmp(event->x(),event->y());
        restrictPoint(pointTmp);

        if(autoModel)                      //[Auto Model]
        {
            if(rectState==IN_PROCESS)
            {
                rect.width=abs(pointTmp.x-rect.x);
                rect.height=abs(pointTmp.y-rect.y);
                rect.x=rect.x>pointTmp.x?pointTmp.x:rect.x;
                rect.y=rect.y>pointTmp.y?pointTmp.y:rect.y;

                if(rect.area()>10) //The Area Of The Rect Should Not Be Too Small
                {
                    mask.setTo(GC_BGD);
                    (mask(rect)).setTo(Scalar(GC_PR_FGD));
                    rectState=SET;
                }
                else
                {
                    resetAMParameters();
                }


            }

            vector<Point>::const_iterator it;
            if(lblsState==IN_PROCESS&&isf)
            {
                for(it=fgdPxls.begin();it!=fgdPxls.end();++it)
                    circle(mask,*it,2,GC_FGD,-1);
                lblsState=SET;
            }

            if(lblsState==IN_PROCESS&&!isf)
            {
                for(it=bgdPxls.begin();it!=bgdPxls.end();++it)
                    circle(mask,*it,2,GC_BGD,-1);
                lblsState=SET;
            }

            if(prLblsState==IN_PROCESS&&isf)
            {
                for(it=prFgdPxls.begin();it!=prFgdPxls.end();++it)
                    circle(mask,*it,2,GC_PR_FGD,-1);
                prLblsState=SET;
            }

            if(prLblsState==IN_PROCESS&&!isf)
            {
                for(it=prBgdPxls.begin();it!=prBgdPxls.end();++it)
                    circle(mask,*it,2,GC_PR_BGD,-1);
                prLblsState=SET;
            }

        }
        else                               //[Manual Model]
        {

        }

        drawMouseMove=false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(drawMouseMove)
    {
        Point pointTmp(event->x(),event->y());
        restrictPoint(pointTmp);

        if(autoModel)                     //[Auto Model]
        {
            if(rectState==IN_PROCESS)
            {
                img_cur.copyTo(img_cur_copy);
                rectangle(img_cur_copy,Point(rect.x,rect.y),Point(pointTmp.x,pointTmp.y),Scalar(0,255,0),2);
                displayMat(img_cur_copy);
            }

            if(lblsState==IN_PROCESS&&isf)
            {
                fgdPxls.push_back(pointTmp);
                circle(img_cur_copy,pointTmp,2,Scalar(0,0,255),-1);
                displayMat(img_cur_copy);
            }

            if(lblsState==IN_PROCESS&&!isf)
            {
                bgdPxls.push_back(pointTmp);
                circle(img_cur_copy,pointTmp,2,Scalar(255,0,0),-1);
                displayMat(img_cur_copy);
            }

            if(prLblsState==IN_PROCESS&&isf)
            {
                prFgdPxls.push_back(pointTmp);
                circle(img_cur_copy,pointTmp,2,Scalar(230,130,255),-1);
                displayMat(img_cur_copy);
            }

            if(prLblsState==IN_PROCESS&&!isf)
            {
                prBgdPxls.push_back(pointTmp);
                circle(img_cur_copy,pointTmp,2,Scalar(255,255,160),-1);
                displayMat(img_cur_copy);
            }
        }
        else                              //[Manual Model]
        {
            contour.push_back(pointTmp);
            drawConnects();
        }
    }
}


bool MainWindow::readImage()
{
    contour.clear();

    imgInputPath.sprintf(inputPath.toStdString().c_str(),frameIdx);
    img_cur=imread(imgInputPath.toStdString());
    if(!img_cur.empty())
    {
        img_cur.copyTo(img_pre);
        displayMat(img_cur);

        prSetDlg->setStartFrame(QString("%1").arg(frameIdx));

        mask.create(img_cur.size(),CV_8UC1);

        return true;
    }
    else
    {
        QMessageBox::information(NULL,"Read Failure","Failed To Read "+imgInputPath,QMessageBox::Ok,QMessageBox::Ok);
        return false;
    }

}


void MainWindow::drawConnects()
{
    img_pre.copyTo(img_cur);
    for(int i=0;i<contour.size();i++)
    {
        circle(img_cur,contour.at(i),0,Scalar(0,0,255));
    }

    for(int i=1;i<contour.size();i++) // i=0;i<int(contour.size())-1;i++ ; Note Int ype
    {
        line(img_cur,contour.at(i-1),contour.at(i),Scalar(0,0,255));
    }

    displayMat(img_cur);
}


void MainWindow::contourFill()
{
    if(!contour.empty())
    {
        vector<vector<Point>> contours;
        if(!useHull)
        {
            approxPolyDP(contour,contour,0,true);
        }
        else
        {
            convexHull(Mat(contour),contour);
        }
        contours.push_back(contour);

        img_pre.copyTo(img_cur);
        drawContours(img_cur, contours, -1, Scalar(0,0,255), CV_FILLED);
        displayMat(img_cur);
        img_cur.copyTo(img_pre);
        img_cur.copyTo(img_cur_copy);

        contour.clear();
    }
}

void MainWindow::getBinMask(const Mat& comMask, Mat& binMask)
{
    if (comMask.empty() || comMask.type() != CV_8UC1)
        CV_Error(CV_StsBadArg, "ComMask Is Empty Or Has Incorrect Type (Not CV_8UC1)");
    if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
        binMask.create(comMask.size(), CV_8UC1);
    binMask = comMask & 1;
}

void MainWindow::resetAMParameters()
{
    if (!mask.empty())
        mask.setTo(Scalar::all(GC_BGD));
    bgdPxls.clear(); fgdPxls.clear();
    prBgdPxls.clear();  prFgdPxls.clear();

    isInitialized = false;
    rectState = NOT_SET;
    lblsState = NOT_SET;
    prLblsState = NOT_SET;
}

void MainWindow::restrictPoint(Point & _point)
{
    _point.x-=validRegion.x;
    _point.y-=validRegion.y;

    _point.x=_point.x>0?_point.x:0;
    _point.y=_point.y>0?_point.y:0;

    _point.x=_point.x<validRegion.width?_point.x:validRegion.width;
    _point.y=_point.y<validRegion.height?_point.y:validRegion.height;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    float xscale=ui->label->frameGeometry().width()/float(validRegion.width);
    float yscale=ui->label->frameGeometry().height()/float(validRegion.height);

    if(!contour.empty())
    {
        for(int i=0;i<contour.size();i++)
        {
            contour.at(i).x*=xscale;
            contour.at(i).y*=yscale;
        }
    }

    if(rectState==SET)
    {
        rect.x*=xscale;
        rect.y*=yscale;
        rect.width*=xscale;
        rect.height*=yscale;
    }

    vector<Point>::iterator it;
    for(it=fgdPxls.begin();it!=fgdPxls.end();++it)
    {
        (*it).x*=xscale;
        (*it).y*=yscale;
    }
    for(it=bgdPxls.begin();it!=bgdPxls.end();++it)
    {
        (*it).x*=xscale;
        (*it).y*=yscale;
    }
    for(it=prFgdPxls.begin();it!=prFgdPxls.end();++it)
    {
        (*it).x*=xscale;
        (*it).y*=yscale;
    }
    for(it=prBgdPxls.begin();it!=prBgdPxls.end();++it)
    {
        (*it).x*=xscale;
        (*it).y*=yscale;
    }

    validRegion.x=ui->label->frameGeometry().x();
    validRegion.y=ui->label->frameGeometry().y()+10;
    validRegion.height=ui->label->frameGeometry().height();
    validRegion.width=ui->label->frameGeometry().width();

    if(!img_cur.empty()&&!img_pre.empty())
    {
        cv::Size sz=cv::Size(validRegion.width,validRegion.height);
        cv::resize(img_pre,img_pre,sz);
        cv::resize(img_cur,img_cur,sz);
    }

    if(!mask.empty()&&!img_cur_copy.empty())
    {
        cv::Size sz=cv::Size(validRegion.width,validRegion.height);
        cv::resize(mask,mask,sz);
        cv::resize(img_cur_copy,img_cur_copy,sz);
    }

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    curTime=QDateTime::currentDateTime();
    qint64 elapsedSec=startTime.msecsTo(curTime)/1000;
    ui->frameshowLabel->setText(QString("%1  %2  %3:%4:%5").arg(frameIdx).arg(frameIdx-startIdx).arg(elapsedSec/3600).arg((elapsedSec%3600)/60).arg(elapsedSec%60));
}

