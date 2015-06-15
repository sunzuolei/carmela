#ifndef COMMAND_H
#define COMMAND_H

#define TIMES_RESEND_IF_NO_REPLY 3      ///< The times that remind user input command
#define TIMEOUT_NO_REPLY_MS      2000   ///< A value will be used in setting time that user has no reply
//for command type:
#define STOP              0x00          ///< The robot stop moving.
#define MOVE_FORWORD      0x01          ///< The robot moves forward.
#define MOVE_BACKWORD     0x02          ///< The robot moves backward
#define TURN_RIGHT        0x03          ///< The robot moves right
#define TURN_LEFT         0x04          ///< The robot moves left
#define CLOSE_REPORT      0x40          ///< A type of command that can be used to set the lower computer closing reporting data to upper computer
#define REPORT_POSITION   0x41          ///< A type of command that can be used to set the lower computer reporting position data to upper computer
#define REPORT_ORIGINAL   0x42          ///< A type of command that can be used to set the lower computer reporting original data to upper computer
#define REPORT_POS_ORI    0x43          ///< A type of command that can be used to set the lower computer reporting position and original data to upper computer at the same time.
//for command reconstruction:
#define CMD_HEADER        0x7E          ///< Start flag
#define CMD_VERSION       0x31          ///< Version
#define GUEST_ADD         0x01          ///< Guest address
#define PC_ADD            0x00          ///< PC address
#define CSF_SEND_CMD      0xFF          ///< A command flag that PC use it to send command to lower computer
#define CMD_TAIL          0x0D          ///< End flag
//for data parsing:
#define MIN_CMD_LEN       8             ///< The minimum length of command
//for reply data parsing
#define RSS_CORRECT       0x00          ///< A command flag that the lower computer use it to feedback signal and data.
#define RSS_INCORRECT     0x04          ///< A type of incorrect feedback signal.When the lower computer cann't parse the command sended by upper computer,it will send a feedback signal to upper computer.
#define RSF_INCORRECT     0xEE          ///< A type of incorrect command feedback signal. The lower computer incorrectly parse commands what the upper computer sends,then the lower computer sends a feedback signal to upper computer.

#endif // COMMAND_H
