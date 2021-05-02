#ifndef MSGTYPE_H
#define MSGTYPE_H

#include <deque>
#include <sstream>
#include <iomanip>
#include "protocol/protocol.h"
#include <QMetaType>
#include <QTime>
#include <QDate>


Q_DECLARE_METATYPE(goodok::command::msg_text_t)
Q_DECLARE_METATYPE(goodok::command::chat_t)

inline goodok::DateTime get_current_DateTime() {
    QDate cd = QDate::currentDate();
    QTime ct = QTime::currentTime();
    goodok::Date date(cd.day(), cd.month(), cd.year());
    goodok::Time time(ct.second(), ct.minute(), ct.hour());

    goodok::DateTime dt(time, date);
    return dt;
}

inline std::string get_time_to_gui(int hours, int minutes) {
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << minutes;

    return ss.str();
}
#endif // MSGTYPE_H
