//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// date_functions.cpp
//
// Identification: /peloton/src/expression/date_functions.cpp
//
// Copyright (c) 2015-2017, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "expression/date_functions.h"

#include <date/date.h>

#include "common/logger.h"
#include "expression/abstract_expression.h"
#include "type/types.h"

namespace peloton {
namespace expression {

uint32_t GetYear(const uint64_t &timestamp) {
  return timestamp / 100000000000 % 10000;
}

uint32_t GetMonth(const uint64_t &timestamp) {
  return timestamp / 1000000000000000 / 27 / 32; 
}

uint32_t GetDay(const uint64_t &timestamp) {
  return timestamp / 1000000000000000 / 27 % 32;
}

uint32_t GetDOY(const uint64_t &timestamp) {
  uint32_t year = GetYear(timestamp);
  uint32_t month = GetMonth(timestamp);
  uint32_t day = GetDay(timestamp);

  // two ugly arrays of manually accumulated days before each month
  uint32_t acc_day[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  uint32_t acc_day_lunar[12] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

  if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
    return acc_day_lunar[month - 1] + day;
  } else {
    return acc_day[month - 1] + day;
  }
}

// The arguments are contained in the args vector
// (1) The first argument is the part of the date to extract
// (see DatePartType in type/types.h
// (2) The second argument is the timestamp to extract the part from
// @return The Value returned should be a type::DecimalValue that is
// constructed using type::ValueFactory
type::Value DateFunctions::Extract(const std::vector<type::Value>& args) {
  DatePartType date_part = args[0].GetAs<DatePartType>();
  uint64_t timestamp = args[1].GetAs<uint64_t>();

  LOG_INFO("Extracting %s from '%s' with raw val %" PRIu64 "", DatePartTypeToString(date_part).c_str(),
           type::ValueFactory::GetTimestampValue(timestamp).ToString().c_str(), timestamp);

  type::Value result;

  // HACK HACK HACK
  // These values are hardcoded for project #1 to pass the test case
  // You should replace all of this with your own implementation
  // Be sure to add all of the DatePartType members!
  // These are hardcoded for "2017-01-01 12:13:14.999999+00"
  switch (date_part) {
    case DatePartType::MILLENNIUM: {
      uint32_t year = GetYear(timestamp);
      result = type::ValueFactory::GetDecimalValue((year - 1) / 1000 + 1);
      break;
    }
    case DatePartType::CENTURY: {
      uint32_t year = GetYear(timestamp);

      // assuming year is always positive
      result = type::ValueFactory::GetDecimalValue((year - 1) / 100 + 1);
      break;
    }
    case DatePartType::DECADE: {
      uint32_t year = GetYear(timestamp);
      result = type::ValueFactory::GetDecimalValue(year / 10);
      break;
    }
    case DatePartType::DOW: {
      uint32_t doy = GetDOY(timestamp);
      uint32_t year = GetYear(timestamp);

      // // Magic number for each month
      // uint32_t magic[13] = {0, 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};

      // uint32_t dow = year % 100 / 4 + GetDay(timestamp) + magic[month];

      // // Subtract 1 for January or February of a leap year
      // if (((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) && (month == 1 || month == 2)) {
      //   dow -= 1;
      // }

      // dow = (dow + year % 100) % 7;

      uint32_t magic[28] = {5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4};

      result = type::ValueFactory::GetDecimalValue((magic[year % 28] + doy - 1) % 7);
      break;
    }
    case DatePartType::DOY: {
      result = type::ValueFactory::GetDecimalValue(GetDOY(timestamp));

      break;
    }
    case DatePartType::YEAR: {
      uint32_t year = GetYear(timestamp);
      result = type::ValueFactory::GetDecimalValue(year);
      break;
    }
    case DatePartType::QUARTER: {
      result = type::ValueFactory::GetDecimalValue((GetMonth(timestamp) - 1) / 3 + 1);
      break;
    }
    case DatePartType::WEEK: {
      int magic[28] = {-4, -2, -1, 0, 1, -4, -3, -2, -1, 1, 2, -4, -3, -1, 0, 1, 2, -3, -2, -1, 0, 2, -4, -3, -2, 0, 1, 2};

      int doy = GetDOY(timestamp);
      uint32_t year = GetYear(timestamp);

      int week = doy + magic[year % 28];

      if (week < 0) {
        switch (magic[year % 28]) {
          case -4:
            week = 53;
            break;
          case -3:
            week = ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 53 : 52;
          default:
            week = 52;
            break;
        }
      } else {
        week = week / 7 + 1;
      }

      result = type::ValueFactory::GetDecimalValue(week);
      break;
    }
    case DatePartType::MONTH: {
      uint32_t month = GetMonth(timestamp);
      result = type::ValueFactory::GetDecimalValue(month);
      break;
    }
    case DatePartType::DAY: {
      uint32_t day = GetDay(timestamp);
      result = type::ValueFactory::GetDecimalValue(day);
      break;
    }
    case DatePartType::HOUR: {
      uint32_t hour = (timestamp / 1000000 % 100000) / 3600;
      result = type::ValueFactory::GetDecimalValue(hour);
      break;
    }
    case DatePartType::MINUTE: {
      uint32_t minute = (timestamp / 1000000 % 100000) / 60 % 60;
      result = type::ValueFactory::GetDecimalValue(minute);
      break;
    }
    case DatePartType::SECOND: {
      uint32_t second = (timestamp / 1000000 % 100000) % 60;
      uint32_t micro = timestamp % 1000000;
      result = type::ValueFactory::GetDecimalValue(double(second) + double(micro) / 1000000);
      break;
    }
    case DatePartType::MILLISECOND: {
      uint32_t second = (timestamp / 1000000 % 100000) % 60;
      uint32_t micro = timestamp % 1000000;
      result = type::ValueFactory::GetDecimalValue(double(second * 1000) + double(micro) / 1000);
      break;
    }
    case DatePartType::MICROSECOND: {
      uint32_t second = (timestamp / 1000000 % 100000) % 60;
      uint32_t micro = timestamp % 1000000;
      result = type::ValueFactory::GetDecimalValue(second * 1000000 + micro);
      break;
    }
    default: {
      result = type::ValueFactory::GetNullValueByType(type::Type::DECIMAL);
    }
  };

  return (result);
}

}  // namespace expression
}  // namespace peloton
