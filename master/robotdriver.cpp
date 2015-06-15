/**
 * @file  robotdriver.cpp
 * @brief The robotdriver.cpp is a file that contains the specific process realized.
 * @data  <Nov.21.2013>
 * @version <4.0>
 */

#include "robotdriver.h"

#include <QFile>

/**
 * @brief Instantiate and initialize the RobotDriver function. You are only needed to input the serial port name.
 * @param portName The type is Qstring. Set the serial port name such as COM1.
 * @exception none
 * @note The default setting is as followed. The band rate is 9600,the flow control is off, the parity
 * is none parity,the data bites is 8 bites and the stop bits is 2 bits.
 */
RobotDriver::RobotDriver(const QString & portName)
{
    this->port_  = new QextSerialPort(portName, QextSerialPort::EventDriven);
    this->timer_ = new QTimer(this);
    timer_->setInterval(1000); //1000ms == 1s

    port_->setBaudRate(BAUD9600);     //PLS modify the configuration based on our case.
    port_->setFlowControl(FLOW_OFF);  //PLS refer to: http://blog.csdn.net/free2011/article/details/5999325
    port_->setParity(PAR_NONE);
    port_->setDataBits(DATA_8);
    port_->setStopBits(STOP_2);
    resendCounter_ = 0;
    newIdx_ = 0;
    flag=false;
    //qDebug()<<"ready??";              //as printf() function

    if (port_->open(QIODevice::ReadWrite) == true)
    {
        connect(port_, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(port_, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)));
        connect(timer_, SIGNAL(timeout()), this, SLOT(resend()));
       /* if (!(port_->lineStatus() & LS_DSR))
        {qCritical() << "The serial port is not turned on, the driver is down..." ;
            exit(EXIT_FAILURE);
        }
        qDebug() << "Listening gust's msg on " << port_->portName().toAscii();*/
    }
    else
    {
       qCritical() << "Serial port failed to open:" << port_->errorString().toAscii();
       exit(EXIT_FAILURE);
    }
}

RobotDriver::~RobotDriver()
{;}
/**
 * @brief It is a overload function.Send command type to lower computer to control the robot moving.
 * @param comType  It is the command type.By which upper computer send command
 *        to lower computer to control the robot moving/reporting data and so on.
 * @see  void RobotDriver::sendCommand(const char *cmd, const char &cmdLen) and
 *       void RobotDriver::sendCommand(const QByteArray & cmdArray)
 * @note The command type contains move forward/move backward/move right/move left/
 *       report position data/report original data/ report position and original data/
 *       close reporting data
 */
void RobotDriver::sendCommand(const char &comType)
{
    if (comType != STOP             &&
        comType != MOVE_FORWORD     &&
        comType != MOVE_BACKWORD    &&
        comType != TURN_RIGHT       &&
        comType != TURN_LEFT        &&
        comType != CLOSE_REPORT     &&
        comType != REPORT_POSITION  &&
        comType != REPORT_ORIGINAL  &&
        comType != REPORT_POS_ORI)
    {
        qCritical()<< "RobotDriver::sendCommand error: the entry "
                   << int(comType) << " is invalid!";
//        if(comType != STOP             ||
//           comType != MOVE_FORWORD     ||
//           comType != MOVE_BACKWORD    ||
//           comType != TURN_RIGHT       ||
//           comType != TURN_LEFT        ||
////         comType != CLOSE_REPORT)
//            REPORT_CYCLE=0x00;
//        else
//            REPORT_CYCLE=0x05;
    }
    qDebug() << comType;

    QByteArray cmd;
    cmd.append(CMD_HEADER);
    cmd.append(CMD_VERSION);
    cmd.append(GUEST_ADD);
    cmd.append(comType);
    cmd.append(CSF_SEND_CMD);
//    cmd.append(1); //ldz ADD
    cmd.append(reportCycle);
    char checkSum = CMD_VERSION + GUEST_ADD + comType + CSF_SEND_CMD+reportCycle;
    cmd.append(checkSum);
    cmd.append(CMD_TAIL);

    cmdBackup_ = cmd;
    port_->write(cmd);
    timer_->start(TIMEOUT_NO_REPLY_MS);  //TIMEOUT_NO_REPLY_MS=2000ms
}

/**
 * @brief It is a overload function.Send command to lower computer to respond.The command is cmdLen size.
 * @param cmd The command is needed to send.
 * @param cmdLen The size of command.
 * @see  void RobotDriver::sendCommand(const char &comType) and void RobotDriver::sendCommand(const QByteArray & cmdArray)
 */
void RobotDriver::sendCommand(const char *cmd, const char &cmdLen)
{
    //TODO: exception detection, to ensure the command is reasonable.
    cmdBackup_ = QByteArray(cmd, cmdLen);
    port_ -> write(cmd, cmdLen);
    timer_->start(TIMEOUT_NO_REPLY_MS);
}

/**
 * @brief It is a overload function.It send data to lower computer.
 * @param cmdArray It is an array of bytes,containing data from head to tail.
 * @see   void RobotDriver::sendCommand(const char *cmd, const char &cmdLen) and void RobotDriver::sendCommand(const char &comType)
 */
void RobotDriver::sendCommand(const QByteArray & cmdArray)
{
    cmdBackup_ = QByteArray(cmdArray);
    port_ -> write(cmdArray);
    timer_->start(TIMEOUT_NO_REPLY_MS);
}

/**
 * @brief Parse the feedback data from the lower computer and check the data whether it is available.The function will store data if it detects the command.
 */

void RobotDriver::onReadyRead()
{
    QFile file("file.txt");
//    QString fileName=time.toString("yyyyMMddhhmmss");
//    fileName.append(".txt");
//    Qfile file;
//    file.setFileName(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    int len = port_->bytesAvailable();
    int sz  = len + newIdx_;
    replyData_.resize(sz);

    port_->read(replyData_.data()+newIdx_, len);
    qDebug() << len << " bytes received:"
             << replyData_;
    /*TODO: display replyData_ as ASCII,
    so it is always readable.*/
    newIdx_ = 0;

    //Start package parsing.
    const char rep[] = {CMD_HEADER, CMD_VERSION, PC_ADD};
    QByteArray sub2 = QByteArray::fromRawData(rep, 2);
    QByteArray sub3 = QByteArray::fromRawData(rep, 3);

    int startPart = 0;
    int findSub3 = 0;
    int n3 = replyData_.count(sub3);
    int paraLen = 0;
    int i3 = 0;

    if (n3 == 0)       //Not contain Header+Version+PC_ADD at the same time
    {
        qDebug() << "\'Header+version+address\' isn't contained.";
        int i2 = replyData_.lastIndexOf(sub2);
        if ( i2 != -1 )                          //Cotaining header + version
        {
            qDebug() << "\'Header+version\' is contained.";
            if (sz - i2 > 2)
            {
               qDebug() << "Invalid data follows. Give up!";
               return;
            }
            startPart = i2;
            qDebug() << "A potential msg is partly catched,"\
                        " Waiting for next pack...";
        }
        else                                //Uncotaining header + version
        {
            qDebug() << "\'Header+version\' isn't' contained.";
            int i1 = replyData_.lastIndexOf(CMD_HEADER);
            if( i1 == -1 )
            {
                qDebug() << "\'Header\' isn't contained. Give up!";
                return;
            }

            else qDebug() << "\'Header\' is contained";
            if (sz - i1 > 1)
            {
                qDebug() << "Invalid data follows. Give up";;
                return;
            }
            startPart = i1;
            qDebug() << "A potential msg is partly catched,"\
                        " Waiting for next pack...";
        }
    }
    else            //contain "Header+version+PC_ADD"
    {
        qDebug() << "\'Header+version+address\' is contained.";

        //Checking a package whether contains several "Header+version+PC_ADD"
        while( n3-- )  //n3= replyData_.count(sub3)
        {
            i3 = replyData_.indexOf(sub3, findSub3);
            if (sz - i3 < 6)
            {
                startPart = i3;
                qDebug() << "The length is less than 6, "\
                            "\'data length\' is unavailable. "\
                            "Waiting for next pack...";
                break;
            }
            qDebug() << "\'data length\' is available.";
            paraLen = replyData_[i3 + 5];
            if ( i3 + paraLen + 8 > sz )
            {
                startPart = i3;
                qDebug() << "Too short to construct a msg."\
                            " Waiting for next pack...";
                break;
            }
            qDebug() << "The pack is long enough.";
            startPart = sz;
            if ( replyData_[i3 + paraLen + 7] != (char)CMD_TAIL )
            {
                findSub3 = i3 + 1;       //findSub3=version
                qDebug() << "Msg tail doesn't coincide.";
            }
            else
            {
                char checksum = 0;
                for (int t = i3+1; t < i3+paraLen+6; t++)                   //count the checksum
                    checksum = checksum + replyData_[t];
                if ( replyData_[i3+paraLen+6] != checksum )                 //checksum is invalid
                {
                    qDebug() << "Checksum doesn't coincide.";
                    findSub3 = i3 + 1;

                }
                else                                                        //checksum is valid
                {
                    qDebug() << "Everything is fine. Start parsing...";
                    if ( replyData_[i3 + 3] == (char)RSF_INCORRECT )        //RSF=0xEE stands for the contral cmd
                                                                            //is wrong
                    {
                        qDebug() << "Guest received a polluted cmd,"\
                                    " Resending now...";
                        resendCounter_ = 0;
                        timer_->start(TIMEOUT_NO_REPLY_MS);
                    /*NOTE: only resend the last cmd here, it might
                     be problematic if error occured in more than one cmds consecutively.*/
                    }
                    else if(replyData_[i3 + 4] == (char)RSS_CORRECT )       //begin to check the RSS=0x00 and the RSS=0x00 here
                         {
                            timer_->stop();

                            if(replyData_[i3 + 3] == (char)CLOSE_REPORT )    //Close reporting data to PC
                            {
                                qDebug() << "Close reporting data to PC";
                            }

                            else if(replyData_[i3 + 3] == (char)REPORT_POSITION ) //Only report position data
                            {
                                if (paraLen%2!=1)
                                qDebug() << "Something must be wrong, paraLen is not even number!";

                                qDebug() << "Start to report position data!";
                                QDateTime time = QDateTime::currentDateTime ();  //insert the system time currently
                                qDebug() << "Time stamp: " << time.toString();
                                stream.seek(file.size());
                                stream<< time.toString("yyyyMMdhhmmss.z");
                                stream<<" ";

                                Motion newMot;
                                newMot.timeStamp = time;

                                for (int p = 0; p < paraLen ; p++ )
                                {
                                    int m=((int)replyData_[i3+6+p]);
                                   float mo = ((int)replyData_[i3+6+p] << 8) | (unsigned char)replyData_[i3+7+p];
 //                                float mot = ((int)replyData_[i3+6+p] << 24) | (int)replyData_[i3+7+p]<< 16| (int)replyData_[i3+8+p]<< 8| (unsigned char)replyData_[i3+9+p];

                                   if(p==0)
                                   {
                                       newMot.heartSignal= m ;
                                       qDebug() << "Heart signal:" <<m << " " ;
                                   }

                                   if (p==1)                 //X 2 bytes
                                   {
                                       newMot.xPosition = mo;
                                       qDebug() << "X: " << mo  << " ";
                                   }
                                   else if (p==3)                 //Y 2 bytes
                                   {
                                       newMot.yPosition= mo;
                                       qDebug() << "Y: " << mo  << " ";
                                   }

                                   else if (p==5)                 ////Orientation 2 bytes 航向角 相对于起点转动角度
                                   {
                                       newMot.orientation= mo;
                                       qDebug() << "Orientation: " << mo  << " ";
                                   }

                                   else if (p==7)                 //Translation 2 bytes
                                   {
                                       newMot.translation = mo;
                                       qDebug() << "Translation: " << mo  << " cm";
                                   }

                                   else if (p==9)
                                   {
                                       newMot.rotation = mo;        //Rotation 2 bytes
                                       qDebug() << "Rotation: " << mo  << "/10 degree";
                                   }

                                   if (p==0)
                                       stream<<(qint8)m;

                                   if(p==1||p==3||p==5||p==7)
                                        stream<<(qint32)mo;

//                                   if(p==9||p==11)
//                                      stream<<(qint32)mo;

                                   if(p==9)
                                        stream<<(qint32)(mo/10);

                                   stream<<" ";
                                }
                                stream<<"\n";
                                motionLog_.enqueue(newMot);
                                emit motionComes(newMot);
                            }

                            else if(replyData_[i3 + 3] == (char)REPORT_ORIGINAL )   //Only to report original data
                            {
                                if (paraLen%2!=1)
                                qDebug() << "Something must be wrong, paraLen is not even number!";

                                qDebug() << "Start to report original data!";
                                QDateTime time = QDateTime::currentDateTime ();  //insert the system time currently
                                qDebug() << "Time stamp: " << time.toString();
                                stream.seek(file.size());
                                stream<< time.toString("yyyyMMdhhmmss.z");
                                stream<<" ";

                                Motion newMot;
                                newMot.timeStamp = time;

                                for (int p = 0; p < paraLen ; p++ )
                                {
                                   int m=((int)replyData_[i3+6+p]);
                                   float mo = ((int)replyData_[i3+6+p] << 8) | (unsigned char)replyData_[i3+7+p];
                                   int mot =(int)replyData_[i3+6+p];

                                   if(p==0)
                                   {
                                       newMot.heartSignal= m ;
                                       qDebug() << "Heart signal:" << m << " " ;
                                   }

                                   if (p==1)                 //left_front 2 bytes
                                   {
                                       newMot.leftFront= mo;
                                       qDebug() << "left_front " << mo  << " ";
                                   }

                                   else if (p==3)                 //left_front 2 bytes
                                   {
                                       newMot.leftBack= mo;
                                       qDebug() << "left_back " << mo  << " ";
                                   }

                                   else if (p==5)                 //left_front 2 bytes
                                   {
                                       newMot.rightFront= mo;
                                       qDebug() << "right_front " << mo  << " ";
                                   }

                                   else if (p==7)                 //left_front 2 bytes
                                   {
                                       newMot.rightBack= mo;
                                       qDebug() << "right_back " << mo  << " ";
                                   }

                                   else if(p==9)
                                   {
                                       newMot.leftSide= mot;
                                       qDebug() <<"leftside" << mot << " ";
                                   }

                                   else if(p==10)
                                   {
                                       newMot.rightSide= mot;
                                       qDebug() <<"rightside"<< mot <<" ";
                                   }

                                   if (p==0)
                                       stream<<(qint8)m;

                                   if(p==1||p==3||p==5||p==7)
                                      stream<<(qint32)mo;

                                   if(p==9||p==10)
                                       stream<< (qint8)mot;

                                   stream<<" ";

                                }
                                stream<<"\n";
                                motionLog_.enqueue(newMot);
                                emit motionComes(newMot);
                            }

                            else if(replyData_[i3 + 3] == (char)REPORT_POS_ORI)  //report position and original data at the same time
                            {
                                if (paraLen%2!=1)
                                qDebug() << "Something must be wrong, paraLen is not even number!";

                                qDebug() << "Start to report position and original data together!";
                                QDateTime time = QDateTime::currentDateTime ();  //insert the system time currently
                                qDebug() << "Time stamp: " << time.toString();
                                stream.seek(file.size());
                                stream<< time.toString("yyyyMMdhhmmss.z");
                                stream<<" ";

                                Motion newMot;
                                newMot.timeStamp = time;

                                for (int p = 0; p < paraLen ; p++ )
                                {
                                   int m =(int)replyData_[i3+6+p];
                                   float mo = ((int)replyData_[i3+6+p] << 8) | (unsigned char)replyData_[i3+7+p];
//                                 float mot = ((int)replyData_[i3+6+p] << 24) | (int)replyData_[i3+7+p]<< 16| (int)replyData_[i3+8+p]<< 8| (unsigned char)replyData_[i3+9+p];

                                   if(p==0)
                                   {
                                       newMot.heartSignal= m ;
                                       qDebug() << "Heart signal:" <<m << " " ;
                                   }

                                   if (p==1)                 //left_front 2 bytes
                                   {
                                       newMot.leftFront= mo;
                                       qDebug() << "left_front " << mo  << " ";
                                   }

                                   else if (p==3)                 //left_front 2 bytes
                                   {
                                       newMot.leftBack= mo;
                                       qDebug() << "left_back " << mo  << " ";
                                   }

                                   else if (p==5)                 //left_front 2 bytes
                                   {
                                       newMot.rightFront= mo;
                                       qDebug() << "right_front " << mo  << " ";
                                   }

                                   else if (p==7)                 //left_front 2 bytes
                                   {
                                       newMot.rightBack= mo;
                                       qDebug() << "right_back " << mo  << " ";
                                   }

                                   else if(p==9)                  //lsftside 1 byte
                                   {
                                       newMot.leftSide= m;
                                       qDebug() <<"leftside" << m << " ";
                                   }

                                   else if(p==10)                 //rightside 1 byte
                                   {
                                       newMot.rightSide= m;
                                       qDebug() <<"rightside"<< m <<" ";
                                   }
                                   else if (p==11)                 //X 2 bytes
                                   {
                                       newMot.translation = mo;
                                       qDebug() << "X: " << mo  << " ";
                                   }
                                   else if (p==13)                 //Y 2 bytes
                                   {
                                       newMot.rotation= mo;
                                       qDebug() << "Y: " << mo  << " ";
                                   }

                                   else if (p==15)                 //O 2 bytes 相对于起点转动角度
                                   {
                                       newMot.rotation= mo;
                                       qDebug() << "O: " << mo  << " ";
                                   }

                                   else if (p==17)                 //Translation 2 bytes
                                   {
                                       newMot.translation = mo;
                                       qDebug() << "Translation: " << mo  << " cm";
                                   }

                                   else if (p==19)
                                   {
                                       newMot.rotation = mo;        //Rotation 2 bytes
                                       qDebug() << "Rotation: " << mo  << "/10 degree";
                                   }


//                                 if(p==11||p==16)
//                                      stream<<(qint64)mot;

                                   if(p==1||p==3||p==5||p==7||p==11||p==13||p==15||p==17||p==19)
                                        stream<<(qint32)mo;
                                   if(p==0||p==9||p==10)
                                       stream<<(qint8)m;
                                   if(p==23)
                                        stream<<(qint32)(mo/10);

                                   stream<<" ";
                                }
                                stream<<"\n";
                                motionLog_.enqueue(newMot);        //Adds value newMot to the tail of the queue
                                emit motionComes(newMot);

                            }

                         }
                         else{                                              //judging the RSS=0x04?
                                if(replyData_[i3 + 4] == (char)RSS_INCORRECT) //checking the RSS=0x04 ressend cmd again
                                {
                                    qDebug()<<"The cmd of reporting data is polluted,"\
                                              "Resending again";
                                    resendCounter_ = 0;
                                    timer_->start(TIMEOUT_NO_REPLY_MS);
                                }
                                else                                        //
                                {
                                    qDebug()<<"Waiting for the next package";
                                    startPart = i3+2;
                                    break;
                                }

                              }
                }
             }

           }

    }


    file.close();

    /*The left data might construct a potential command with
    part of the next frame data, so following trick moves
    them to the head of the buffer.*/
    if ( startPart == sz )
        return;

    QByteArray partData = replyData_.mid(startPart);
    replyData_.replace(0, sz, partData);
    newIdx_ = partData.size();
}

/**
 * @brief Detect and display the serial port is opend or closed.
 */
void RobotDriver::onDsrChanged(bool status)                          //状态判断
{
    if (status)
        qDebug() << "device was turned on" << endl;
    else
        qDebug() << "device was turned off" << endl;
}

 /**
  * @brief PC Resend the command to lower computer.
  * @note It will work ,to a great extent,if the lower computer understands command sending by PC incorrectly .
  */
void RobotDriver::resend()
{
    if (resendCounter_ < TIMES_RESEND_IF_NO_REPLY)
    {
        resendCounter_++;
        port_-> write(cmdBackup_);
        qDebug() << "Resend command due to timeout: "
                 << TIMEOUT_NO_REPLY_MS
                 << " microseconds!";
    }
    else
    {
        timer_->stop();
        resendCounter_ = 0;
        qDebug() << "The command has been resent "
                 << TIMES_RESEND_IF_NO_REPLY
                 << " times, but guest replied nothing. "\
                    "Is guest alive?";
    }
}

/**
 * @brief Return the robot position message.
 */
Motion &RobotDriver::getMotion()
{
    static Motion mo;
    if( !motionLog_.isEmpty() )
    {
        mo = motionLog_.dequeue();
        return mo;
    }

    mo.timeStamp = QDateTime::fromString("0", "s");
    mo.translation = 0;
    mo.rotation = 0;
    return mo;
}

