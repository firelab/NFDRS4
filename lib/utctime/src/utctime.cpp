/*!
 * \file        utctime.cpp
 * \brief       Implementation of UTCTime() class and associated functions.
 * \details     Implementation of UTCTime() class and associated functions.
 * \author      Paul Griffiths
 * \copyright   Copyright 2013 Paul Griffiths. Distributed under the terms
 * of the GNU General Public License. <http://www.gnu.org/licenses/>
 */


#include <sstream>
#include <ctime>
#include <cassert>
#include "utctime.h"

using namespace utctime;
using std::time;
using std::difftime;
using std::mktime;
using std::gmtime;
using std::localtime;
using std::strftime;


/********************************************************************
 *
 * UTCTime class member function definitions.
 *
 ********************************************************************/


/*!
 * \brief       Default constructor.
 * \details     Default constructor, which initializes to the current time.
 * \throws      bad_time_init if unable to get the current time.
 */

UTCTime::UTCTime() :
        m_year(0), m_month(0), m_day(0),
        m_hour(0), m_minute(0), m_second(0),
        m_timestamp(0) {
    m_timestamp = std::time(0);
    if ( m_timestamp == -1 ) {
        throw bad_time_init();
    }

    std::tm* ptm = gmtime(&m_timestamp);
    if ( ptm == 0 ) {
        throw bad_time_init();
    }

    m_year = ptm->tm_year + 1900;
    m_month = ptm->tm_mon + 1;
    m_day = ptm->tm_mday;
    m_hour = ptm->tm_hour;
    m_minute = ptm->tm_min;
    m_second = ptm->tm_sec;
}


/*!
 * \brief       Constructor taking a std::tm struct.
 * \details     Constructor taking a std::tm struct.
 * \param utc_tm A std::tm struct containing the desired initialization time.
 * \throws      bad_time_init if unable to get the current time.
 * \throws      invalid_date if the supplied date is bad.
 */

UTCTime::UTCTime(const std::tm& utc_tm) :
        m_year(utc_tm.tm_year + 1900), m_month(utc_tm.tm_mon + 1),
        m_day(utc_tm.tm_mday), m_hour(utc_tm.tm_hour),
        m_minute(utc_tm.tm_min), m_second(utc_tm.tm_sec),
        m_timestamp(0) {
    try {
        validate_date(m_year, m_month, m_day, m_hour, m_minute, m_second);
    } catch(invalid_date) {
        throw;
    }

    try {
        m_timestamp = get_utc_timestamp(m_year, m_month, m_day,
                                        m_hour, m_minute, m_second);
    } catch(bad_time) {
        throw bad_time_init();
    }
}


/*!
 * \brief       Constructor taking individual date values.
 * \details     Constructor taking individual date values.
 * \param year The year
 * \param month The month, 1 to 12
 * \param day The day, 1 to 31, depending on month
 * \param hour The hour, 0 to 23
 * \param minute The minute, 0 to 59
 * \param second The second, 0 to 59. Leap seconds are not supported.
 * \throws      bad_time_init if unable to get the current time.
 * \throws      invalid_date if the supplied date is bad.
 */

UTCTime::UTCTime(const int year, const int month,
                 const int day, const int hour,
                 const int minute, const int second) :
        m_year(year), m_month(month), m_day(day),
        m_hour(hour), m_minute(minute), m_second(second),
        m_timestamp(0) {
    try {
        validate_date(m_year, m_month, m_day, m_hour, m_minute, m_second);
    } catch(invalid_date) {
        throw;
    }

    try {
        m_timestamp = get_utc_timestamp(m_year, m_month, m_day,
                                        m_hour, m_minute, m_second);
    } catch(bad_time) {
        throw bad_time_init();
    }
}


/*!
 * \brief       Returns a std::tm struct containing the UTC datetime.
 * \details     Returns a std::tm struct containing the UTC datetime.
 * \returns     A std::tm struct containing the UTC datetime.
 */

std::tm UTCTime::get_tm() const {
    std::tm ret_tm;
    ret_tm.tm_year = m_year - 1900;
    ret_tm.tm_mon = m_month - 1;
    ret_tm.tm_mday = m_day;
    ret_tm.tm_hour = m_hour;
    ret_tm.tm_min = m_minute;
    ret_tm.tm_sec = m_second;
    ret_tm.tm_yday = get_day_of_year(m_year, m_month, m_day);
    return ret_tm;
}


/*!
 * \brief       Returns a std::string containing the UTC datetime.
 * \details     Returns a std::string containing the UTC datetime.
 * \returns     A std::string containing the UTC datetime.
 * \throws      bad_time if unable to get the time.
 */

std::string UTCTime::time_string() const {
    std::tm* utc_tm = gmtime(&m_timestamp);
    if ( utc_tm == 0 ) {
        throw bad_time();
    }
    char tstring[100];
    strftime(tstring, 1000, "%A %B %d, %Y %H:%M:%S UTC", utc_tm);
    return tstring;
}


/*!
 * \brief       Returns a std::string containing the UTC datetime in RFC3339
 * format.
 * \details     Returns a std::string containing the UTC datetime in RFC3339
 * format.
 * \returns     A std::string containing the UTC datetime in RFC3339 format.
 * \throws      bad_time if unable to get the time.
 */

std::string UTCTime::time_string_inet() const {
    std::tm* utc_tm = gmtime(&m_timestamp);
    if ( utc_tm == 0 ) {
        throw bad_time();
    }
    char tstring[100];
    strftime(tstring, 1000, "%Y-%m-%dT%H:%M:%SZ", utc_tm);
    return tstring;
}


/*!
 * \brief       Returns a time_t timestamp for the UTC datetime.
 * \details     Returns a time_t timestamp for the UTC datetime.
 * \returns     A time_t timestamp for the UTC datetime.
 */

time_t UTCTime::timestamp() const {
    return m_timestamp;
}


/********************************************************************
 *
 * Standalone function definitions.
 *
 ********************************************************************/

/*!
*\brief       Calculates day of year (1- 366)
* \details     Calculates day of year (1- 366)
* \param year The year
* \param month The month, 1 to 12
* \param day The day, 1 to 31, depending on the month
* \returns     Julian day of year (1 - 366)
*/
 
int utctime::get_day_of_year(int year, int month, int day)
{
    static const int days_in_month[] = { 31, 28, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31 };

    int dayOfYear = 0;
    for (int m = 0; m < month - 1; m++)
        dayOfYear += days_in_month[m];
    dayOfYear += day;
    if (is_leap_year(year) && month > 2)
        dayOfYear++;
    return dayOfYear;
}


/*!
 * \brief       Checks whether a supplied date is valid.
 * \details     Checks whether a supplied date is valid.
 * \param year The year
 * \param month The month, 1 to 12
 * \param day The day, 1 to 31, depending on the month
 * \param hour The hour, 0 to 23
 * \param minute The minute, 0 to 59
 * \param second The second, 0 to 59. Leap seconds are not supported.
 * \returns     true if the date if valid.
 * \throws      invalid_date if the date is not valid.
 * \todo        Why does this throw an exception? Why not return false?
 * In order so that the error message can show why it's invalid?
 */

bool utctime::validate_date(const int year, const int month,
                            const int day, const int hour,
                            const int minute, const int second) {
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31};
    static const char* month_names[] = {"January", "February", "March",
                                        "April", "May", "June", "July",
                                        "August", "September", "October",
                                        "November", "December"};

    if ( year == 0 ) {
        std::ostringstream oss;
        oss << "Invalid year: " << year;
        throw invalid_date(oss.str());
    } else if ( month < 1 || month > 12 ) {
        std::ostringstream oss;
        oss << "Invalid month: " << month;
        throw invalid_date(oss.str());
    } else if ( day < 1 ) {
        std::ostringstream oss;
        oss << "Invalid day: " << day;
        throw invalid_date(oss.str());
    } else if ( day > days_in_month[month - 1] &&
                !(month == 2 && day == 29 && is_leap_year(year)) ) {
        std::ostringstream oss;
        oss << "Invalid day: " << day << " of " << month_names[month - 1]
            << ", " << year;
        throw invalid_date(oss.str());
    } else if ( hour < 0 || hour > 23 ) {
        std::ostringstream oss;
        oss << "Invalid hour: " << hour;
        throw invalid_date(oss.str());
    } else if ( minute < 0 || minute > 59 ) {
        std::ostringstream oss;
        oss << "Invalid minute: " << minute;
        throw invalid_date(oss.str());
    } else if ( second < 0 || second > 59 ) {
        std::ostringstream oss;
        oss << "Invalid second: " << second;
        throw invalid_date(oss.str());
    }

    return true;
}


/*!
 * \brief       Checks if a UTC timestamp is accurate.
 * \details     Checks if a UTC timestampe is accurate. A time_t timestamp
 * is computed from the supplied datetime elements, and compared to the
 * supplied time_t timestamp. The difference between the two, in seconds,
 * is stored in the supplied secs_diff argument. This function is needed
 * because the methodology used to calculate a timestamp by this library
 * can sometimes be inaccurate when leap seconds or other unpredictable
 * calendar changes occur. We therefore need a method to check if the
 * returned timestamp is accurate. Other functions provided in this
 * library call this function, so the user should not normally need to
 * call it.
 * \param check_time The time_t timestamp to check
 * \param secs_diff Modified to contain the difference, in seconds
 * \param year The year
 * \param month The month, 1 to 12
 * \param day The day, 1 to 31, depending on the month
 * \param hour The hour, 0 to 23
 * \param minute The minute, 0 to 59
 * \param second The second, 0 to 59
 * \returns     true if the supplied timestamp is accurate, false otherwise
 * \throws      bad_time if the current time cannot be obtained.
 */

bool utctime::check_utc_timestamp(const time_t check_time, int& secs_diff,
                                  const int year, const int month,
                                  const int day, const int hour,
                                  const int minute, const int second) {
    std::tm* ptm = gmtime(&check_time);
    if ( ptm == 0 ) {
        throw bad_time();
    }

    std::tm check_tm = *ptm;
    bool agrees = false;
    if ( check_tm.tm_year + 1900 == year &&
         check_tm.tm_mon + 1 == month &&
         check_tm.tm_mday == day &&
         check_tm.tm_hour == hour &&
         check_tm.tm_min == minute &&
         check_tm.tm_sec == second ) {
        agrees = true;
    }

    if ( agrees == false ) {
        std::tm utc_tm;
        utc_tm.tm_year = year - 1900;
        utc_tm.tm_mon = month - 1;
        utc_tm.tm_mday = day;
        utc_tm.tm_hour = hour;
        utc_tm.tm_min = minute;
        utc_tm.tm_sec = second;

        secs_diff = tm_intraday_secs_diff(utc_tm, check_tm);
    } else {
        secs_diff = 0;
    }

    return agrees;
}


/*!
 * \brief       Returns a time_t interval representing one day.
 * \details     Returns a time_t interval representing one day. The C
 * standard does not define the units in which a time_t value is measured.
 * On POSIX-compliant systems it is measured in seconds, but we cannot
 * assume this for full portability.
 * \returns     A time_t interval representing one day.
 * \throws      bad_time if the current time cannot be obtained.
 */

/*
 *  The function works by setting up two struct tms a day apart and
 *  calculating the difference between the values that mktime() yields.
 *
 *  We've picked January 2 and January 3 as the dates to use, since
 *  we're likely clear of any DST or other weirdness on these dates.
 *  Since mktime() will modify the struct we pass to it if it represents
 *  a bad date, and since we reuse it, it should be good anyway.
 *
 *  The utctime::get_hour_diff() and utctime::get_sec_diff() functions
 *  work in a similar way.
 */

time_t utctime::get_day_diff() {
    std::tm datum_day;
    datum_day.tm_sec = 0;
    datum_day.tm_min = 0;
    datum_day.tm_hour = 12;
    datum_day.tm_mday = 2;
    datum_day.tm_mon = 0;
    datum_day.tm_year = 103;
    datum_day.tm_isdst = -1;

    const time_t datum_time = mktime(&datum_day);
    if ( datum_time == -1 ) {
        throw bad_time();
    }

    datum_day.tm_mday += 1;
    const time_t tomorrow_time = mktime(&datum_day);
    if ( tomorrow_time == -1 ) {
        throw bad_time();
    }

    return (tomorrow_time - datum_time);
}


/*!
 * \brief       Returns a time_t interval representing one hour.
 * \details     Returns a time_t interval representing one hour. The C
 * standard does not define the units in which a time_t value is measured.
 * On POSIX-compliant systems it is measured in seconds, but we cannot
 * assume this for full portability.
 * \returns     A time_t interval representing one hour.
 * \throws      bad_time if the current time cannot be obtained.
 */

time_t utctime::get_hour_diff() {
    std::tm datum_day;
    datum_day.tm_sec = 0;
    datum_day.tm_min = 0;
    datum_day.tm_hour = 12;
    datum_day.tm_mday = 2;
    datum_day.tm_mon = 0;
    datum_day.tm_year = 103;
    datum_day.tm_isdst = -1;

    const time_t datum_time = mktime(&datum_day);
    if ( datum_time == -1 ) {
        throw bad_time();
    }

    datum_day.tm_hour += 1;
    const time_t next_hour_time = mktime(&datum_day);
    if ( next_hour_time == -1 ) {
        throw bad_time();
    }

    return (next_hour_time - datum_time);
}


/*!
 * \brief       Returns a time_t interval representing one second.
 * \details     Returns a time_t interval representing one second. The C
 * standard does not define the units in which a time_t value is measured.
 * On POSIX-compliant systems it is measured in seconds, but we cannot
 * assume this for full portability.
 * \returns     A time_t interval representing one second.
 * \throws      bad_time if the current time cannot be obtained.
 */

time_t utctime::get_sec_diff() {
    std::tm datum_day;
    datum_day.tm_sec = 0;
    datum_day.tm_min = 0;
    datum_day.tm_hour = 12;
    datum_day.tm_mday = 2;
    datum_day.tm_mon = 0;
    datum_day.tm_year = 103;
    datum_day.tm_isdst = -1;

    const time_t datum_time = mktime(&datum_day);
    if ( datum_time == -1 ) {
        throw bad_time();
    }

    datum_day.tm_sec += 1;
    const time_t next_sec_time = mktime(&datum_day);
    if ( next_sec_time == -1 ) {
        throw bad_time();
    }

    return (next_sec_time - datum_time);
}


/*!
 * \brief       Compares two std::tm structs.
 * \details     Compares two std::tm structs. Only the year, month, day,
 * hour, minute and second are compared. Any timezone or DST information
 * is ignored.
 * \param first The first std::tm struct.
 * \param second The second std::tm struct.
 * \returns     -1 if `first` is earlier than `second`, 1 if `first` is later
 * than `second`, and 0 if `first` is equal to `second`.
 */

int utctime::tm_compare(const std::tm& first, const std::tm& second) {
    int compare_result;

    if ( first.tm_year != second.tm_year ) {
        compare_result = first.tm_year > second.tm_year ? 1 : -1;
    } else if ( first.tm_mon != second.tm_mon ) {
        compare_result = first.tm_mon > second.tm_mon ? 1 : -1;
    } else if ( first.tm_mday != second.tm_mday ) {
        compare_result = first.tm_mday > second.tm_mday ? 1 : -1;
    } else if ( first.tm_hour != second.tm_hour ) {
        compare_result = first.tm_hour > second.tm_hour ? 1 : -1;
    } else if ( first.tm_min != second.tm_min ) {
        compare_result = first.tm_min > second.tm_min ? 1 : -1;
    } else if ( first.tm_sec != second.tm_sec ) {
        compare_result = first.tm_sec > second.tm_sec ? 1 : -1;
    } else {
        compare_result = 0;
    }

    return compare_result;
}


/*!
 * \brief       Returns the difference between two std::tm structs.
 * \details     Returns the difference between two std::tm structs. The
 * structs are assumed to be within 24 hours of each other, and if they
 * are not, the returned result is computed as if they were. For instance,
 * comparing 10:00 on one day to 14:00 on the next day will yield a
 * difference equivalent to 4 hours, not to 28 hours.
 * \param first The first std::tm struct
 * \param second The second std::tm struct
 * \returns The difference, in seconds, between the two std::tm structs.
 * The difference is positive if `first` is earlier than `second`, and
 * negative if `second` is earlier than `first`.
 */

int utctime::tm_intraday_secs_diff(const std::tm& first,
                                   const std::tm& second) {
    static const int secs_in_day = 86400;
    static const int secs_in_hour = 3600;
    static const int secs_in_min = 60;

    const int time_comp = tm_compare(first, second);
    int difference = 0;

    if ( time_comp == 0 ) {
        difference = 0;
    } else {
        difference = (second.tm_hour - first.tm_hour) * secs_in_hour;
        difference += (second.tm_min - first.tm_min) * secs_in_min;
        difference += (second.tm_sec - first.tm_sec);

        if ( time_comp == 1 && difference > 0 ) {
            difference -= secs_in_day;
        } else if ( time_comp == -1 && difference < 0 ) {
            difference += secs_in_day;
        }
    }
    
    return difference;
}


/*!
 * \brief       Checks if the supplied year is a leap year.
 * \details     Checks if the supplied year is a leap year.
 * \param year A year
 * \returns     `true` is `year` is a leap year, `false` otherwise.
 */

bool utctime::is_leap_year(const int year) {
    bool leap_year = false;
    if ( year % 4 == 0 &&
         (year % 100 != 0 ||
          year % 400 == 0) ) //{
        leap_year = true;
    //}// else {
     //   leap_year = false;
    //}
    return leap_year;
}


/*!
 * \brief       Adds one or more days to a std::tm struct.
 * \details     Adds one or more days to a std::tm struct, incrementing
 * the month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of days to add
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_increment_day(std::tm* changing_tm, const int quantity) {
    enum months {january, february, march, april, may, june, july,
                 august, september, october, november, december};

    if ( quantity < 0 ) {
        tm_decrement_day(changing_tm, -quantity);
    } else {
        int num_days = quantity;

        while ( num_days-- ) {
            changing_tm->tm_mday += 1;
            switch ( changing_tm->tm_mon ) {
                case january:
                case march:
                case may:
                case july:
                case august:
                case october:
                    if ( changing_tm->tm_mday > 31 ) {
                        changing_tm->tm_mday = 1;
                        changing_tm->tm_mon += 1;
                    }
                    break;

                case december:
                    if ( changing_tm->tm_mday > 31 ) {
                        changing_tm->tm_mday = 1;
                        changing_tm->tm_mon = 0;
                        if ( changing_tm->tm_year == -1 ) {
                            changing_tm->tm_year = 1;
                        } else {
                            changing_tm->tm_year += 1;
                        }
                    }
                    break;

                case april:
                case june:
                case september:
                case november:
                    if ( changing_tm->tm_mday > 30 ) {
                        changing_tm->tm_mday = 1;
                        changing_tm->tm_mon +=1;
                    }
                    break;

                case 1:
                    if ( changing_tm->tm_mday > 29 ) {
                        changing_tm->tm_mday = 1;
                        changing_tm->tm_mon += 1;
                    } else if ( changing_tm->tm_mday > 28 &&
                                !is_leap_year(changing_tm->tm_year) ) {
                        changing_tm->tm_mday = 1;
                        changing_tm->tm_mon += 1;
                    }
                    break;

                default:
                    assert(false);
                    break;
            }
        }
    }

    return changing_tm;
}


/*!
 * \brief       Adds one or more hours to a std::tm struct.
 * \details     Adds one or more hours to a std::tm struct, incrementing
 * the day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of hours to add
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_increment_hour(std::tm* changing_tm, const int quantity) {
    static const int hours_in_day = 24;

    if ( quantity < 0 ) {
        tm_decrement_hour(changing_tm, -quantity);
    } else {
        int num_hours = quantity;

        if ( num_hours >= hours_in_day ||
             num_hours >= hours_in_day - changing_tm->tm_hour ) {
            int num_days = quantity / hours_in_day;
            num_hours -= num_days * hours_in_day;
            if ( num_hours >= hours_in_day - changing_tm->tm_hour ) {
                ++num_days;
                num_hours -= hours_in_day - changing_tm->tm_hour;
                changing_tm->tm_hour = num_hours;
            }
            tm_increment_day(changing_tm, num_days);
        }

        changing_tm->tm_hour += num_hours;
    }

    return changing_tm;
}


/*!
 * \brief       Adds one or more minutes to a std::tm struct.
 * \details     Adds one or more minutes to a std::tm struct, incrementing
 * the hour, day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of minutes to add
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_increment_minute(std::tm* changing_tm,
                                      const int quantity) {
    static const int mins_in_hour = 60;

    if ( quantity < 0 ) {
        tm_decrement_minute(changing_tm, -quantity);
    } else {
        int num_mins = quantity;

        if ( num_mins >= mins_in_hour ||
             num_mins >= mins_in_hour - changing_tm->tm_min ) {
            int num_hours = quantity / mins_in_hour;
            num_mins -= num_hours * mins_in_hour;
            if ( num_mins >= mins_in_hour - changing_tm->tm_min ) {
                ++num_hours;
                changing_tm->tm_min += num_mins - mins_in_hour;
                num_mins = 0;
            }
            tm_increment_hour(changing_tm, num_hours);
        }

        changing_tm->tm_min += num_mins;
    }
    return changing_tm;
}


/*!
 * \brief       Adds one or more seconds to a std::tm struct.
 * \details     Adds one or mor seconds to a std::tm struct, incrementing
 * the minute, hour, day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of seconds to add
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_increment_second(std::tm* changing_tm,
                                      const int quantity) {
    static const int secs_in_min = 60;

    if ( quantity < 0 ) {
        tm_decrement_second(changing_tm, -quantity);
    } else {
        int num_secs = quantity;

        if ( num_secs >= secs_in_min ||
             num_secs >= secs_in_min - changing_tm->tm_sec ) {
            int num_mins = quantity / secs_in_min;
            num_secs -= num_mins * secs_in_min;
            if ( num_secs >= secs_in_min - changing_tm->tm_sec ) {
                ++num_mins;
                changing_tm->tm_sec += num_secs - secs_in_min;
                num_secs = 0;
            }
            tm_increment_minute(changing_tm, num_mins);
        }

        changing_tm->tm_sec += num_secs;
    }

    return changing_tm;
}


/*!
 * \brief       Deducts one or more days from a std::tm struct.
 * \details     Deducts one or more days from a std::tm struct, decrementing
 * the month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of days to deduct
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_decrement_day(std::tm* changing_tm, const int quantity) {
    enum months {january, february, march, april, may, june, july,
                 august, september, october, november, december};

    if ( quantity < 0 ) {
        tm_increment_day(changing_tm, -quantity);
    } else {
        int num_days = quantity;

        while ( num_days-- ) {
            if ( changing_tm->tm_mday > 1 ) {
                changing_tm->tm_mday -= 1;
            } else {
                switch ( changing_tm->tm_mon ) {
                    case january:
                        changing_tm->tm_mday = 31;
                        changing_tm->tm_mon = 11;
                        if ( changing_tm->tm_year == 1 ) {
                            changing_tm->tm_year = -1;
                        } else {
                            changing_tm->tm_year -= 1;
                        }
                        break;

                    case february: 
                    case april:
                    case june:
                    case august:
                    case september:
                    case november:
                        changing_tm->tm_mday = 31;
                        changing_tm->tm_mon -= 1;
                        break;

                    case may:
                    case july:
                    case october:
                    case december:
                        changing_tm->tm_mday = 30;
                        changing_tm->tm_mon -= 1;
                        break;

                    case march:
                        if ( is_leap_year(changing_tm->tm_year) ) {
                            changing_tm->tm_mday = 29;
                        } else {
                            changing_tm->tm_mday = 28;
                        }
                        changing_tm->tm_mon -= 1;
                        break;

                    default:
                        assert(false);
                        break;
                }
            }
        }
    }

    return changing_tm;
}


/*!
 * \brief       Deducts one or more hours from a std::tm struct.
 * \details     Deducts one or more hours from a std::tm struct, decrementing
 * the day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of hours to deduct
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_decrement_hour(std::tm* changing_tm, const int quantity) {
    static const int hours_in_day = 24;

    if ( quantity < 0 ) {
        tm_increment_hour(changing_tm, -quantity);
    } else {
        int num_hours = quantity;

        if ( num_hours >= hours_in_day || num_hours > changing_tm->tm_hour ) {
            int num_days = quantity / hours_in_day;
            num_hours -= num_days * hours_in_day;
            if ( num_hours > changing_tm->tm_hour ) {
                ++num_days;
                num_hours -= changing_tm->tm_hour;
                changing_tm->tm_hour = hours_in_day;
            }
            tm_decrement_day(changing_tm, num_days);
        }

        changing_tm->tm_hour -= num_hours;
    }

    return changing_tm;
}


/*!
 * \brief       Deducts one or more minutes from a std::tm struct.
 * \details     Deducts one or more minutes from a std::tm struct, decrementing
 * the hour, day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of minutes to deduct
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_decrement_minute(std::tm* changing_tm,
                                      const int quantity) {
    static const int mins_in_hour = 60;

    if ( quantity < 0 ) {
        tm_increment_minute(changing_tm, -quantity);
    } else {
        int num_mins = quantity;

        if ( num_mins >= mins_in_hour || num_mins > changing_tm->tm_min ) {
            int num_hours = quantity / mins_in_hour;
            num_mins -= num_hours * mins_in_hour;
            if ( num_mins > changing_tm->tm_min ) {
                ++num_hours;
                num_mins -= changing_tm->tm_min;
                changing_tm->tm_min = mins_in_hour;
            }
            tm_decrement_hour(changing_tm, num_hours);
        }

        changing_tm->tm_min -= num_mins;
    }
    return changing_tm;
}


/*!
 * \brief       Deducts one or more seconds from a std::tm struct.
 * \details     Deducts one or more seconds from a std::tm struct, decrementing
 * the minute, hour, day, month and/or the year as necessary.
 * \param changing_tm A pointer to the std::tm struct to increment. The
 * struct referred to by the pointer is modified by the function.
 * \param quantity The number of seconds to deduct
 * \returns A pointer to the same std::tm struct.
 */

std::tm* utctime::tm_decrement_second(std::tm* changing_tm,
                                      const int quantity) {
    static const int secs_in_min = 60;

    if ( quantity < 0 ) {
        tm_increment_second(changing_tm, -quantity);
    } else {
        int num_secs = quantity;

        if ( num_secs >= secs_in_min || num_secs > changing_tm->tm_sec ) {
            int num_mins = quantity / secs_in_min;
            num_secs -= num_mins * secs_in_min;
            if ( num_secs > changing_tm->tm_sec ) {
                ++num_mins;
                num_secs -= changing_tm->tm_sec;
                changing_tm->tm_sec = secs_in_min;
            }
            tm_decrement_minute(changing_tm, num_mins);
        }

        changing_tm->tm_sec -= num_secs;
    }

    return changing_tm;
}


/*!
 * \brief       Gets a time_t timestamp for a requested UTC time.
 * \details     Gets a time_t timestamp for a requested UTC time.
 * \param year The year
 * \param month The month, 1 to 12
 * \param day The day, 1 to 31, depending on the month
 * \param hour The hour, 0 to 23
 * \param minute The minute, 0 to 59
 * \param second The second, 0 to 59. Leap seconds are not supported.
 * \returns     A time_t timestamp for the requested UTC time.
 * \throws      bad_time if the current time cannot be obtained.
 */

time_t utctime::get_utc_timestamp(const int year, const int month,
                                  const int day, const int hour,
                                  const int minute, const int second) {

    //  Create a struct tm containing the desired UTC time.

    std::tm local_tm;
    local_tm.tm_sec = second;
    local_tm.tm_min = minute;
    local_tm.tm_hour = hour;
    local_tm.tm_mday = day;
    local_tm.tm_mon = month - 1;        // Months start at 0 for tm
    local_tm.tm_year = year - 1900;     // Years since 1900 for tm
    local_tm.tm_isdst = 0;

    //  Get a timestamp close to (i.e. within 24 hours of) the
    //  desired UTC time.

    time_t utc_ts = mktime(&local_tm);
    if ( utc_ts == -1 ) {
        throw bad_time();
    }

    //  Compute the difference with the desired UTC time...

    int secs_diff = get_utc_timestamp_sec_diff(utc_ts, year, month,
                                               day, hour, minute, second);

    //  ...and adjust the timestamp, if needed.

    if ( secs_diff ) {
        const time_t one_sec = get_sec_diff();
        utc_ts -= one_sec * secs_diff;

        secs_diff = get_utc_timestamp_sec_diff(utc_ts, year, month, day,
                                               hour, minute, second);

        if ( secs_diff ) {

            //  We're pretty unlucky if we get here, but let's check
            //  for a leap second on either side, and give up if not.

            if ( get_utc_timestamp_sec_diff(utc_ts + one_sec, year, month,
                                            day, hour, minute, second) == 0 ) {
               utc_ts += one_sec;
            } else if ( get_utc_timestamp_sec_diff(utc_ts - one_sec, year,
                                     month, day, hour, minute, second) == 0 ) {
               utc_ts -= one_sec;
            } else { 
                throw bad_time();
            }
        }
    }

    return utc_ts;
}


/*!
 * \brief       Checks a time_t timestamp against a UTC time.
 * \details     Checks a time_t timestamp against a UTC time, and returns
 * the difference in seconds. This function only returns a good value
 * if the timestamp is less than 24 hours away from the desired time,
 * so the caller is responsible for making sure that it is. This function
 * may also return a bad value if a leap second or other unpredictable
 * calendar change falls between the desired UTC time and the provided
 * time stamp. The result should therefore always be checked with
 * utctime::check_utc_timestamp(), or by calling this function again.
 * \param check_time The time_t timestamp to check
 * \param year The year
 * \param month The month, 1 to 12
 * \param day The day, 1 to 31, depending on the month
 * \param hour The hour, 0 to 23
 * \param minute The minute, 0 to 59
 * \param second The second, 0 to 59. Leap seconds are not supported.
 * \returns     The difference, if any, represented in seconds.
 * \throws      bad_time if the current time cannot be obtained.
 */

int utctime::get_utc_timestamp_sec_diff(const time_t check_time,
                                        const int year, const int month,
                                        const int day, const int hour,
                                        const int minute, const int second) {

    //  Get a struct tm representing UTC time for the provided
    //  timestamp.

    std::tm* ptm = gmtime(&check_time);
    if ( ptm == 0 ) {
        throw bad_time();
    }
    std::tm check_tm = *ptm;

    //  Create a second struct tm representing the desired UTC time.

    std::tm utc_tm;
    utc_tm.tm_year = year - 1900;
    utc_tm.tm_mon = month - 1;
    utc_tm.tm_mday = day;
    utc_tm.tm_hour = hour;
    utc_tm.tm_min = minute;
    utc_tm.tm_sec = second;

    //  Compare the two and return the difference.

    return tm_intraday_secs_diff(utc_tm, check_tm);
}


/*!
 * \brief       Overloaded less than operator
 * \details     Overloaded less than operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is less than `rhs`, false otherwise.
 */

bool UTCTime::operator<(const UTCTime& rhs) const {
    bool less_than = false;
    if ( m_year != rhs.m_year ) {
        less_than = m_year < rhs.m_year;
    } else if ( m_month != rhs.m_month ) {
        less_than = m_month < rhs.m_month;
    } else if ( m_day != rhs.m_day ) {
        less_than = m_day < rhs.m_day;
    } else if ( m_hour != rhs.m_hour ) {
        less_than = m_hour < rhs.m_hour;
    } else if ( m_minute != rhs.m_minute ) {
        less_than = m_minute < rhs.m_minute;
    } else if ( m_second != rhs.m_second ) {
        less_than = m_second < rhs.m_second;
    }
    return less_than;
}


/*!
 * \brief       Overloaded greater than or equal to operator
 * \details     Overloaded greater than or equal to operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is greater than or equal to `rhs`,
 * false otherwise.
 */

bool UTCTime::operator>=(const UTCTime& rhs) const {
    return ((*this < rhs) == false);
}


/*!
 * \brief       Overloaded greater than operator
 * \details     Overloaded greater than operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is greater than `rhs`, false otherwise.
 */

bool UTCTime::operator>(const UTCTime& rhs) const {
    bool greater_than = false;
    if ( m_year != rhs.m_year ) {
        greater_than = m_year > rhs.m_year;
    } else if ( m_month != rhs.m_month ) {
        greater_than = m_month > rhs.m_month;
    } else if ( m_day != rhs.m_day ) {
        greater_than = m_day > rhs.m_day;
    } else if ( m_hour != rhs.m_hour ) {
        greater_than = m_hour > rhs.m_hour;
    } else if ( m_minute != rhs.m_minute ) {
        greater_than = m_minute > rhs.m_minute;
    } else if ( m_second != rhs.m_second ) {
        greater_than = m_second > rhs.m_second;
    }
    return greater_than;
}


/*!
 * \brief       Overloaded less than or equal to operator
 * \details     Overloaded less than or equal to operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is less than or equal to `rhs`,
 * false otherwise.
 */

bool UTCTime::operator<=(const UTCTime& rhs) const {
    return ((*this > rhs) == false);
}


/*!
 * \brief       Overloaded equality operator
 * \details     Overloaded equality operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is equal to `rhs`, false otherwise.
 */

bool UTCTime::operator==(const UTCTime& rhs) const {
    bool equal = false;
    if ( m_year == rhs.m_year &&
         m_month == rhs.m_month &&
         m_day == rhs.m_day &&
         m_hour == rhs.m_hour &&
         m_minute == rhs.m_minute &&
         m_second == rhs.m_second ) {
        equal = true;
    }
    return equal;
}


/*!
 * \brief       Overloaded inequality operator
 * \details     Overloaded inequality operator
 * \param rhs The UTCTime instance to which to compare
 * \returns     true if the instance is not equal to `rhs`, false otherwise.
 */

bool UTCTime::operator!=(const UTCTime& rhs) const {
    return ((rhs == *this) == false);
}


/*!
 * \brief       Overloaded subtraction operator
 * \details     Overloaded subtraction operator
 * \param rhs The UTCTime instance to subtract from the instance
 * \returns     The difference, in seconds, between the two instances.
 */

double UTCTime::operator-(const UTCTime& rhs) const {
    return std::difftime(m_timestamp, rhs.m_timestamp);
}
