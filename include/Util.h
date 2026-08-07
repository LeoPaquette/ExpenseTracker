#ifndef EXPENSETRACKER_UTIL_H
#define EXPENSETRACKER_UTIL_H

#include <string>
#include <regex>

namespace util {

extern const std::regex DATE_PATTERN;

extern const int DAYS_IN_MONTH[];


constexpr bool isYearLeapYear(const int year)  {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool parseDate(const std::string& value, int* outYear, int* outMonth, int* outDay);

int compareDates(const std::string& a, const std::string& b);

inline bool insensitive_equals(std::string a, std::string b);

}

#endif //EXPENSETRACKER_UTIL_H
