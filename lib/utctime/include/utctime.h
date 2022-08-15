/*!
 * \file        utctime.h
 * \brief       Interface to UTCTime() class and associated functions.
 * \details     Interface to UTCTime() class and associated functions.
 * \author      Paul Griffiths
 * \copyright   Copyright 2013 Paul Griffiths. Distributed under the terms
 * of the GNU General Public License. <http://www.gnu.org/licenses/>
 */


#ifndef PG_UTC_TIME_H
#define PG_UTC_TIME_H

#include <string>
#include "time64.h"

//! Namespace for UTCTime() class and associated functions.

namespace utctime {


/*!
 * \brief       Base exception class
 * \details     All custom exceptions thrown by the UTCTime() class and
 * associated functions are derived from UTCTimeException.
 */

class UTCTimeException {
    private:
        std::string m_error_message;

    public:

        /*!
         * \brief       Constructor
         * \param msg An error message to print when `what()` is called.
         */
        explicit UTCTimeException(const std::string msg =
                                  "No error message")
            : m_error_message(msg) {}

        //! Destructor.
        virtual ~UTCTimeException() {}

        /*!
         * \brief       Returns an error message.
         * \returns     A std::string containing the error message.
         */

        const std::string& what() const { return m_error_message; }
};

//!  Thrown when the time cannot be initialized.

class bad_time_init : public UTCTimeException {
    public:

        //! Constructor
        explicit bad_time_init() :
            UTCTimeException("Couldn't initialize time") {};
};

//!  Thrown when the time cannot be obtained.

class bad_time : public UTCTimeException {
    public:

        //! Constructor
        explicit bad_time() :
            UTCTimeException("Couldn't get time") {};
};

//!  Thrown when an invalid date is provided to the constructor

class invalid_date : public UTCTimeException {
    public:

        //! Constructor
        explicit invalid_date(const std::string& msg) :
            UTCTimeException(msg) {};
};


/*
 *  Standalone functions.
 */

bool validate_date(const int year, const int month,
                   const int day, const int hour,
                   const int minute, const int second);
Time64_T get_day_diff();
Time64_T get_hour_diff();
Time64_T get_sec_diff();
int tm_compare(const TM& first, const TM& second);
int tm_intraday_secs_diff(const TM& first, const TM& second);
bool is_leap_year(const int year);
TM* tm_increment_day(TM* changing_tm, const int quantity = 1);
TM* tm_increment_hour(TM* changing_tm, const int quantity = 1);
TM* tm_increment_minute(TM* changing_tm, const int quantity = 1);
TM* tm_increment_second(TM* changing_tm, const int quantity = 1);
TM* tm_decrement_day(TM* changing_tm, const int quantity = 1);
TM* tm_decrement_hour(TM* changing_tm, const int quantity = 1);
TM* tm_decrement_minute(TM* changing_tm, const int quantity = 1);
TM* tm_decrement_second(TM* changing_tm, const int quantity = 1);
bool check_utc_timestamp(const Time64_T check_time, int& secs_diff,
                         const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);
Time64_T get_utc_timestamp(const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);
int get_utc_timestamp_sec_diff(const Time64_T check_time,
                               const int year, const int month,
                               const int day, const int hour,
                               const int minute, const int second);


/*!
 * \brief       A class for holding a UTC time.
 * \details     A class for holding a UTC time.
 */

class UTCTime {
    public:
        explicit UTCTime();
        explicit UTCTime(const TM& utc_tm);
        explicit UTCTime(const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);

        TM get_tm() const;
        std::string time_string() const;
        std::string time_string_inet() const;
        Time64_T timestamp() const;

        bool operator<(const UTCTime& rhs) const;
        bool operator>=(const UTCTime& rhs) const;
        bool operator>(const UTCTime& rhs) const;
        bool operator<=(const UTCTime& rhs) const;
        bool operator==(const UTCTime& rhs) const;
        bool operator!=(const UTCTime& rhs) const;

        double operator-(const UTCTime& rhs) const;

    private:
        int m_year;
        int m_month;
        int m_day;
        int m_hour;
        int m_minute;
        int m_second;
        Time64_T m_timestamp;
};

}           //  namespace utctime

#endif          // PG_UTC_TIME_H
