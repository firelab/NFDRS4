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
#include <ctime>


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
int get_day_of_year(int year, int month, int day);

bool validate_date(const int year, const int month,
                   const int day, const int hour,
                   const int minute, const int second);
time_t get_day_diff();
time_t get_hour_diff();
time_t get_sec_diff();
int tm_compare(const std::tm& first, const std::tm& second);
int tm_intraday_secs_diff(const std::tm& first, const std::tm& second);
bool is_leap_year(const int year);
std::tm* tm_increment_day(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_increment_hour(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_increment_minute(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_increment_second(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_decrement_day(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_decrement_hour(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_decrement_minute(std::tm* changing_tm, const int quantity = 1);
std::tm* tm_decrement_second(std::tm* changing_tm, const int quantity = 1);
bool check_utc_timestamp(const time_t check_time, int& secs_diff,
                         const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);
time_t get_utc_timestamp(const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);
int get_utc_timestamp_sec_diff(const time_t check_time,
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
        explicit UTCTime(const std::tm& utc_tm);
        explicit UTCTime(const int year, const int month,
                         const int day, const int hour,
                         const int minute, const int second);

        std::tm get_tm() const;
        std::string time_string() const;
        std::string time_string_inet() const;
        time_t timestamp() const;

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
        time_t m_timestamp;
};

}           //  namespace utctime

#endif          // PG_UTC_TIME_H
