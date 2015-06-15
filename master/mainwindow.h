#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QPainterPath>
#include <QPoint>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>

#include "robotdriver.h"
#include "Data.h"

namespace Ui {
class mainWindow;
}

class mainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit mainWindow(QWidget *parent = 0);
    ~mainWindow();
public:
    RobotDriver *probot;

public slots:
            void showKinpic();
            void getinfo();
protected:
     virtual bool eventFilter(QObject *watched, QEvent *e);
     void mouseMoveEvent(QMouseEvent *e);
     void mousePressEvent(QMouseEvent *e);
     void paintOnWidget(QWidget *w);
    
private:
    Ui::mainWindow *ui;
    QPixmap imagePixmap;
    QPixmap imagePixmap_2;
    int cn;
    int cnt;

    Data preData;
    Data nextData;
    Data midData;

    Motion mo;

    QPainterPath* path;
    QPoint point;

    QTimer* pTimer;

    QPoint  pos0,last;



private      :
            void drawRTCurve(QPainter *painter);
            void drawRCSystem(QPainter *painter);
            Data compound(const Data wa, const Data ab);
            double piTopi(int angle);
            void starthData();

            /**
             * Key presssed control function.
             */
                void keyPressEvent(QKeyEvent* e);

            /**
              * Key released control function.
              */
                void keyReleaseEvent(QKeyEvent *e);

                void showinfo();



};

#endif // MAINWINDOW_H
