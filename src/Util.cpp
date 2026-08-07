#include "include/Util.h"

#include <algorithm>
#include <ranges>
#include <regex>
#include <unordered_set>


namespace util {

const std::regex DATE_PATTERN(R"(^(\d{4})-(\d{2})-(\d{2})$)");

const int DAYS_IN_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


bool parseDate(const std::string& value, int* outYear, int* outMonth, int* outDay) {
    std::smatch match;
    if (value.empty() || !regex_match(value, match, DATE_PATTERN)) {
        return false;
    }

    const int year = stoi(match[1].str());
    const int month = stoi(match[2].str());
    const int day = stoi(match[3].str());

    int maxDays = DAYS_IN_MONTH[month - 1];
    if (2 == month && isYearLeapYear(year)) {
        maxDays = 29;
    }

    if (1 > day || day > maxDays) {
        return false;
    }

    *outYear = year;
    *outMonth = month;
    *outDay = day;
    return true;
}

int compareDates(const std::string& a, const std::string& b) {
    int aYear = 0;
    int aMonth = 0;
    int aDay = 0;
    if (!parseDate(a, &aYear, &aMonth, &aDay)) {
        throw std::runtime_error("a is not a valid date");
    }

    int bYear = 0;
    int bMonth = 0;
    int bDay = 0;
    if (!parseDate(b, &bYear, &bMonth, &bDay)) {
        throw std::runtime_error("b is not a valid date");
    }

    if (aYear < bYear) {
        return -1;
    }

    if (aYear > bYear) {
        return 1;
    }

    if (aMonth < bMonth) {
        return -1;
    }

    if (aMonth > bMonth) {
        return 1;
    }

    if (aDay < bDay) {
        return -1;
    }

    if (aDay > bDay) {
        return 1;
    }

    return 0;
}

bool insensitive_equals(std::string a, std::string b) {
    return std::ranges::equal(
        a,
        b,
        [](const auto c1, const auto c2) {
            return std::tolower(c1) == std::tolower(c2);
        }
    );
}

std::string str_to_lower(std::string str) {
    std::ranges::transform(str, str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return str;
}


}
