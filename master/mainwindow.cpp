#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "math.h"
#include "command.h"

#include <QPainter>
#include <QMessageBox>

mainWindow::mainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);

    const QString c="COM3";
    probot = new RobotDriver(c);
    connect(probot, SIGNAL(motionComes(Motion)), this, SLOT(getinfo()));
    path = new QPainterPath;
    cn = 1;
    cnt =0;
    preData.setValue(0,0,0);
    nextData.setValue(0,0,0);
    ui->widget->installEventFilter(this);
}

mainWindow::~mainWindow()
{
    delete ui;
    delete probot;
    delete path;

}

void mainWindow::getinfo(){


    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(showKinpic()));
    pTimer->start(30);//这个周期必须小于kinect的采样周期


    mo = probot->getMotion();
    starthData();

    showinfo();

    update();
}

Data mainWindow::compound(const Data wa, const Data ab){

       Data wb={0,0,0};
       wb.x = ab.x*cos(wa.r) - ab.y*sin(wa.r) + wa.x;
       wb.y = ab.y*cos(wa.r) + ab.x*sin(wa.r) + wa.y;
       wb.r = piTopi(wa.r) + ab.r;
       return wb;
}

double mainWindow::piTopi(int angle)
{
  if(angle>0)
  {
     angle=angle%3600;
     if(angle>1800)
        angle=angle-3600;
  }
  if(angle<0)
  {
    angle=angle+(abs(angle)/3600)*3600;
    if(angle<-1800)
        angle=angle+3600;
  }
  return angle;
}


void mainWindow::starthData(){

      nextData.setValue(mo.xPosition,mo.yPosition,mo.rotation);

      //if(cnt<2069){
      midData=compound(preData, nextData);
      preData = midData;

      cnt++;
      //}

}


bool mainWindow::eventFilter(QObject *watched, QEvent *e) {
    if (watched == ui->widget) {
        if (e->type() == QEvent::Paint) {
            paintOnWidget(ui->widget);
            return true;
        }
    }
    return QWidget::eventFilter(watched, e);
}

void mainWindow::mouseMoveEvent(QMouseEvent *e){
    QPoint temp = e->pos();
    const int pos_min_x = ui->widget->pos().x();
    const int pos_max_x = pos_min_x + ui->widget->width();
    const int pos_min_y = ui->widget->pos().y();
    const int pos_max_y = pos_min_y + ui->widget->height();
    if ( temp.x()>pos_min_x && temp.x()<pos_max_x && temp.y()>pos_min_y && temp.y()<pos_max_y )
    {

        QPoint newpos = e->globalPos();
        QPoint upleft = pos0 + newpos - last;
        move(upleft);
    }
}

void mainWindow::mousePressEvent(QMouseEvent *e){
    QPoint temp = e->pos();
    const int pos_min_x = ui->widget->pos().x();
    const int pos_max_x = pos_min_x + ui->widget->width();
    const int pos_min_y = ui->widget->pos().y();
    const int pos_max_y = pos_min_y + ui->widget->height();
    if ( temp.x()>pos_min_x && temp.x()<pos_max_x && temp.y()>pos_min_y && temp.y()<pos_max_y )
    {

        last = e->globalPos();
        pos0 = e->globalPos() - e->pos();
    }
}


void mainWindow::paintOnWidget(QWidget *w) {
       QPainter painter(w);
       painter.setRenderHint(QPainter::Antialiasing);
       int side = qMin(w->width(), w->height());
       painter.scale(side / 400.0, side / 400.0);
       drawRCSystem(&painter);
       drawRTCurve(&painter);

}

void mainWindow::drawRTCurve(QPainter *painter){

    painter->setPen(QColor(255,0,0));
    point.setX((midData.x * 10 +100));
    point.setY(-(midData.y * 10 +100));
    if(cnt==0){
      path->moveTo(point);

    }
    path->lineTo(point);
    painter->drawPath(*path);
    cnt++;
}

void mainWindow::drawRCSystem(QPainter *painter){

    QFontMetrics metrics = painter->fontMetrics();


    int textHeight = metrics.ascent() + metrics.descent();
    int leftWidth = metrics.width(tr("1000")) + 5;
    int rightWidth = metrics.width(tr("(武)"));
    int width = this->size().width() - leftWidth - rightWidth;
    int height = this->size().height() - 4 * textHeight;




    painter->translate(leftWidth, 1.75 * textHeight + height);


    int totalCount = 20;
    int totalCount_x=20;
    int count = 4;
    int count_x=4;
    // float deltaX = width / 12.0f;
    float deltaX=(float)width/count_x;
    float deltaY = (float)height / count;




    painter->drawLine(0, 0, width, 0);

    painter->drawText((deltaX * count_x+1),0,tr(" "));
    for (int i = 1; i <=(count_x+1) ; ++i)
    {
        QString month= QString("%1").arg((i-1) * totalCount_x / count_x);
        int stringWidth = metrics.width(month);


        //if(i != 0)
       // {
            painter->drawLine(deltaX * (i-1), 0, deltaX * (i-1), 4);
       // }

        int monthX = deltaX * i-(stringWidth/ 2);
        painter->drawText(monthX, textHeight+6, month);

    }



    painter->drawLine(0, 0, 0, -height);

    painter->drawText(-metrics.width(tr("H/m")),
                     -(deltaY * count + textHeight / 2 + metrics.descent()),
                     tr(" "));
    for (int i = 1; i <= (count+1); ++i)
    {
        QString value = QString("%1").arg((i-1) * totalCount / count);
        int stringWidth = metrics.width(value);

        painter->drawLine(-4, -(i-1) * deltaY, 0, -(i-1) * deltaY);

        painter->drawText(-stringWidth - 4, -(deltaY * i + textHeight / 2 - metrics.ascent()), value);
    }

}




void mainWindow::showKinpic(){

    QString  path=":/Kinpic/pic";
    path+=QString::number(cn);
    path+=".jpg";
    QString  path_2=":/Kinpic/pic";
    path_2+=QString::number(cn+1);
    path_2+=".jpg";

    if (imagePixmap.load(path)&&imagePixmap_2.load(path_2))
    {
           ui->imageLabel->setPixmap(imagePixmap);
           ui->imagelabel_2->setPixmap(imagePixmap_2);

     }
     else
     {
           //QMessageBox::warning(NULL, "no resource", "no resource");
     }
     ui->imageLabel->setScaledContents(true);
     ui->imagelabel_2->setScaledContents(true);
     ui->imageLabel->show();
     ui->imagelabel_2->show();
     cn+=2;

}


/**
 * @brief It is a key press envent function.
 * @note It is used to check whether any key is pressed.If a key defined is pressed,then it will send command to perform.
 * Controling the robot turning right or left,you should not release the key.
 */
void mainWindow::keyPressEvent(QKeyEvent* e)    //set the key control
{
    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_1)  //"key_crtl+key_1" set the reporting data cycle to 0.5s
    {
        probot->reportCycle=0x05;
        //qDebug()<<"The reporting data cycle is 0.5 s";
        ui->textBrowser->insertPlainText("The reporting data cycle is 0.5 s");
        ui->textBrowser->insertPlainText("\n");
    }

    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_2)  //"key_crtl+key_2" set the reporting data cycle to 1s
    {
        probot->reportCycle=0x0A;
        //qDebug()<<"The reporting data cycle is 1 s";
        ui->textBrowser->insertPlainText("The reporting data cycle is 1 s");
        ui->textBrowser->insertPlainText("\n");
    }

    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_3)  //"key_crtl+key_3" set the reporting data cycle to 2s
    {
        probot->reportCycle=0x14;
        //qDebug()<<"The reporting data cycle is 2 s";
        ui->textBrowser->insertPlainText("The reporting data cycle is 2 s");
        ui->textBrowser->insertPlainText("\n");
    }

    else  probot->reportCycle=0x00;

    switch (e->key())
        {
        case Qt::Key_U:
            probot->sendCommand(MOVE_FORWORD);
            qDebug()<<"MOVE FORWORD";
            ui->textBrowser->insertPlainText("ROBOT MOVE FORWORD");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_J:
            probot->sendCommand(MOVE_BACKWORD);
            //qDebug()<<"MOVE BACKWORD";
            ui->textBrowser->insertPlainText("ROBOT MOVE BACKWORD");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_M:
            probot->sendCommand(STOP);
            break;
        case Qt::Key_C:
            probot->sendCommand(CLOSE_REPORT);
            //qDebug()<<"CLOSE REPORT";
            ui->textBrowser->insertPlainText("ROBOT CLOSE REPORT");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_P:
            probot->sendCommand(REPORT_POSITION);
            //qDebug()<<"REPORT POSITION DATA";
            ui->textBrowser->insertPlainText("REPORT POSITION DATA");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_O:
            probot->sendCommand(REPORT_ORIGINAL);
            //qDebug()<<"REPORT ORIGINAL DATA";
            ui->textBrowser->insertPlainText("REPORT ORIGINAL DATA");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_A:
            probot->sendCommand(REPORT_POS_ORI);
            //qDebug()<<"REPORT POSITION AND ORIGINAL DATA";
            ui->textBrowser->insertPlainText("REPORT ORIGINAL DATA");
            ui->textBrowser->insertPlainText("\n");
            break;
        case Qt::Key_H:
            if (!e->isAutoRepeat())
            {if(!(probot->flag))

                {
                    probot->flag=true;
                    probot->sendCommand(TURN_LEFT);
                    //qDebug()<<"TURN LEFT";
                    ui->textBrowser->insertPlainText("ROBOT TURN LEFT");
                    ui->textBrowser->insertPlainText("\n");
                }
            }

            break;
         case Qt::Key_K:
            if (!e->isAutoRepeat())
            {
                if(!(probot->flag))
                {
                    probot->flag=true;
                    probot->sendCommand(TURN_RIGHT);
                    //qDebug()<<"MOVE_Right";
                    ui->textBrowser->insertPlainText("ROBOT MOVE_Right");
                    ui->textBrowser->insertPlainText("\n");
                }
            }
            else
               e->ignore();
            break;
        }
}

/**
 * @brief It is a key release envent function.
 * @note The robot will stop when the key TURN_RIGHT or TURN_LEFT is released.
 *       But the other keys defined are different ,the robot will go moving except for STOP.
 * When control the robot turning right or left,you should not release the key.
 */

void mainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_1)
        e->ignore();
    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_2)
        e->ignore();
    if (e->modifiers() == (Qt::ControlModifier) && e->key() == Qt::Key_3)
        e->ignore();

    switch (e->key())
    {
    case Qt::Key_H:
        if(!e->isAutoRepeat()&&probot->flag)
        {
            probot->flag=false;
            //sendCommand(MOVE_FORWORD);
            probot->sendCommand(STOP);
           //qDebug()<<"stop MOVE Left,begin to MOVE FORWORD";
           // qDebug()<<"stop move";
            ui->textBrowser->insertPlainText("stop move");
            ui->textBrowser->insertPlainText("\n");
        }
         break;
    case Qt::Key_K:
        if(!e->isAutoRepeat()&&probot->flag)
        {
            probot->flag=false;
            //sendCommand(MOVE_FORWORD);
            probot->sendCommand(STOP);
            //qDebug()<<"stop MOVE Right,begin to MOVE FORWORD";
            //qDebug()<<"stop move";
            ui->textBrowser->insertPlainText("stop move");
            ui->textBrowser->insertPlainText("\n");
        }
         break;
    case Qt::Key_U:
         e->ignore();
         break;
    case Qt::Key_J:
         e->ignore();
         break;
    case Qt::Key_M:
         e->ignore();
         break;
    case Qt::Key_C:
         e->ignore();
         break;
    case Qt::Key_O:
         e->ignore();
         break;
    case Qt::Key_P:
         e->ignore();
         break;
    case Qt::Key_A:
         e->ignore();
         break;
    }
}

void mainWindow::showinfo(){
    ui->textBrowser->insertPlainText("**************************Robot Motion is :*****************************************");
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("timeStamp is :"+mo.timeStamp.toString("hh:mm:ss.zzz"));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("heartSignal is :"+tr("").setNum(mo.heartSignal));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("translation is :"+tr("").setNum(mo.translation));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("rotation is :"+tr("").setNum(mo.rotation));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("orientation is :"+tr("").setNum(mo.orientation));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("xPosition is :"+tr("").setNum(mo.xPosition));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("yPosition is :"+tr("").setNum(mo.yPosition));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("leftFront is :"+tr("").setNum(mo.leftFront));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("leftBack is :"+tr("").setNum(mo.leftBack));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("rightFront is :"+tr("").setNum(mo.rightFront));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("rightBack is :"+tr("").setNum(mo.rightBack));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("leftSide is :"+tr("").setNum(mo.leftSide));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("rightSide is :"+tr("").setNum(mo.rightSide));
    ui->textBrowser->insertPlainText("\n");
    ui->textBrowser->insertPlainText("************************************************************************");
    ui->textBrowser->insertPlainText("\n");
}
