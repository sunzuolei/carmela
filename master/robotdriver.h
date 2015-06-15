/**
 * @file robotdriver.h
 * @brief This is a *.h file that defines functions,variable and so on.
 *
 * @date <Nov.21.2013>
 * @version <4.0>
 *
 * User can use this application to control the robot moving by the keyboard.Because of
 * limited condition, the robot's functions are limited. Keys are used to control the robot
 * moving as followed:key_Up\ key_Down\ key_Right\ key_Left\ key_End\ key_P\ key_O\ key_A\ key_C
 * \ key_Ctrl+key_1\ key_Ctrl+key_2\ key_Ctrl+key_3
 */

#ifndef ROBOTDRIVER_H
#define ROBOTDRIVER_H

#include <QMainWindow>
#include <QObject>

//#include <QEvent>
#include "qextserialport.h"
#include <QTimer>
#include <QtDebug>
#include <QByteArray>
#include <QDateTime>
#include <QQueue>

//#include <QKeyEvent>
#include <QDebug>

#include "command.h"
#include "Data.h"

/**
 *@class RobotDriver
 *@brief The RobotDriver class provides  provides a main application window.
 *
 * A main window provides a framework for building an application's user interface.
 */
class RobotDriver : public QObject
{
    Q_OBJECT

public:
/**
 * RobotDriver() function initializes.Set the serial port name at the same time.
 */
    RobotDriver(const QString & portName);
    ~RobotDriver();



    int reportCycle;                     ///< A cycle used to report data to upper computer
    bool flag;                           ///< bool variable

/**
 * @brief Structure overload function. Use it to send control command
 */
    void sendCommand(const char &cmdType);
    void sendCommand(const char *cmd, const char &cmdLen);
    void sendCommand(const QByteArray &cmdArray);

/**
 * @brief Name a function of Motion
 */
    Motion& getMotion();

private:            
    QextSerialPort *port_;              ///< Set the serial port
    QTimer *timer_;                     ///< It provides repetitive and single-shot timers
    QByteArray cmdBackup_;              ///< Store the command message
    char resendCounter_;                ///< The times of resending command
    unsigned int newIdx_;               ///< The type is unsigned int
    QByteArray replyData_;              ///< replyData_ provides an array of bytes.
    QQueue<Motion> motionLog_;          ///< Constructs a generic container named motionLog_ that provides a Motion queue.

/**
 * Key presssed control function.
 */
//    void keyPressEvent(QKeyEvent* e);

/**
  * Key released control function.
  */
//    void keyReleaseEvent(QKeyEvent *e);


private slots:

/**
 * This function is used to read the data from the device
 */
    void onReadyRead();
/**
 * This function is used to judge the serial port state that is opened or closed.
 */
    void onDsrChanged(bool status);

/**
 * This used to resend the command.
 */
    void resend();

signals:

/**
 * This signal is emitted once every time when the state of a robot is changed.
 */
    void motionComes(Motion newMot);


};

#endif // ROBOTDRIVER_H
