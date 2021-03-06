// Copyright (c) 2012-2020 Wojciech Figat. All rights reserved.

#include "DateTime.h"
#include "TimeSpan.h"
#include "String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Core/Math/Math.h"

const int32 DateTime::DaysPerMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int32 DateTime::DaysToMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

DateTime::DateTime(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond)
{
    ASSERT_LOW_LAYER(Validate(year, month, day, hour, minute, second, millisecond));
    int32 totalDays = 0;
    if (month > 2 && IsLeapYear(year))
        totalDays++;
    year--;
    month--;
    totalDays += year * 365 + year / 4 - year / 100 + year / 400 + DaysToMonth[month] + day - 1;
    Ticks = totalDays * Constants::TicksPerDay
            + hour * Constants::TicksPerHour
            + minute * Constants::TicksPerMinute
            + second * Constants::TicksPerSecond
            + millisecond * Constants::TicksPerMillisecond;
}

void DateTime::GetDate(int32& year, int32& month, int32& day) const
{
    // Based on FORTRAN code in:
    // Fliegel, H. F. and van Flandern, T. C.,
    // Communications of the ACM, Vol. 11, No. 10 (October 1968).

    int32 l = Math::FloorToInt(static_cast<float>(GetJulianDay() + 0.5)) + 68569;
    const int32 n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    int32 i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    int32 j = 80 * l / 2447;
    const int32 k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;

    year = i;
    month = j;
    day = k;
}

int32 DateTime::GetDay() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return day;
}

DayOfWeek DateTime::GetDayOfWeek() const
{
    // January 1, 0001 was a Monday
    return static_cast<DayOfWeek>((Ticks / Constants::TicksPerDay) % 7);
}

int32 DateTime::GetDayOfYear() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    for (int32 currentMonth = 1; currentMonth < month; currentMonth++)
        day += DaysInMonth(year, currentMonth);
    return day;
}

int32 DateTime::GetHour12() const
{
    const int32 hour = GetHour();
    if (hour < 1)
        return 12;
    if (hour > 12)
        return hour - 12;
    return hour;
}

int32 DateTime::GetMonth() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return month;
}

int32 DateTime::GetYear() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return year;
}

int32 DateTime::DaysInMonth(int32 year, int32 month)
{
    ASSERT_LOW_LAYER((month >= 1) && (month <= 12));
    if (month == 2 && IsLeapYear(year))
        return 29;
    return DaysPerMonth[month];
}

int32 DateTime::DaysInYear(int32 year)
{
    return IsLeapYear(year) ? 366 : 365;
}

bool DateTime::IsLeapYear(int32 year)
{
    if ((year % 4) == 0)
    {
        return (((year % 100) != 0) || ((year % 400) == 0));
    }
    return false;
}

DateTime DateTime::Now()
{
    int32 year, month, day, dayOfWeek, hour, minute, second, millisecond;
    Platform::GetSystemTime(year, month, dayOfWeek, day, hour, minute, second, millisecond);
    return DateTime(year, month, day, hour, minute, second, millisecond);
}

DateTime DateTime::NowUTC()
{
    int32 year, month, day, dayOfWeek, hour, minute, second, millisecond;
    Platform::GetUTCTime(year, month, dayOfWeek, day, hour, minute, second, millisecond);
    return DateTime(year, month, day, hour, minute, second, millisecond);
}

bool DateTime::Validate(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond)
{
    return (year >= 1) && (year <= 9999) &&
            (month >= 1) && (month <= 12) &&
            (day >= 1) && (day <= DaysInMonth(year, month)) &&
            (hour >= 0) && (hour <= 23) &&
            (minute >= 0) && (minute <= 59) &&
            (second >= 0) && (second <= 59) &&
            (millisecond >= 0) && (millisecond <= 999);
}

String DateTime::ToString() const
{
    return String::Format(TEXT("{}"), *this);
}

String DateTime::ToFileNameString() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return String::Format(TEXT("{0}_{1:0>2}_{2:0>2}_{3:0>2}_{4:0>2}_{5:0>2}"), year, month, day, GetHour(), GetMinute(), GetSecond());
}
