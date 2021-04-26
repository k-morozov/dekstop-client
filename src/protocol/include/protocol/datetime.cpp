#include "datetime.h"

#include <tuple>

Time& Time::operator= (const std::tm& tm) {
    hours = tm.tm_hour;
    minutes = tm.tm_min;
    seconds = tm.tm_sec;
    return *this;
}

bool operator== (const Time& lhs, const Time& rhs) {
    return lhs.hours == rhs.hours && lhs.minutes == rhs.minutes && lhs.seconds == rhs.seconds;
}

bool operator< (const Time& lhs, const Time& rhs) {
    return std::tuple(lhs.hours, lhs.minutes, lhs.seconds)
         < std::tuple(rhs.hours, rhs.minutes, rhs.seconds);
}


Date& Date::operator= (const std::tm& tm) {
    day = tm.tm_mday;
    month = tm.tm_mon;
    year = tm.tm_year + 1900;
    return *this;
}

bool operator== (const Date& lhs, const Date& rhs) {
    return lhs.day == rhs.day && lhs.month == rhs.month && lhs.year == rhs.year;
}

bool operator< (const Date& lhs, const Date& rhs) {
    return std::tuple(lhs.year, lhs.month, lhs.day)
         < std::tuple(rhs.year, rhs.month, rhs.day);
}


DateTime& DateTime::operator= (const std::tm& tm) {
    time = tm;
    date = tm;
    return *this;
}

bool operator== (const DateTime& lhs, const DateTime& rhs) {
    return lhs.time == rhs.time && lhs.date == rhs.date;
}

bool operator< (const DateTime& lhs, const DateTime& rhs) {
    return std::tuple(lhs.date, lhs.time)
         < std::tuple(rhs.date, rhs.time);
}
