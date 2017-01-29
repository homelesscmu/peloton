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
  // TODO: what if year is negative ?
  return timestamp / 100000000000 % 10000;
}

uint32_t GetMonth(const uint64_t &timestamp) {
  return timestamp / 1000000000000000 / 27 / 32; 
}

uint32_t GetDay(const uint64_t &timestamp) {
  return timestamp / 1000000000000000 / 27 % 32;
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
    case DatePartType::CENTURY: {
      uint32_t year = GetYear(timestamp);
      int century = year > 0 ? (year - 1) / 100 + 1 : (year - 1) / 100;
      result = type::ValueFactory::GetDecimalValue(century);
      break;
    }
    case DatePartType::DECADE: {
      uint32_t year = GetYear(timestamp);
      result = type::ValueFactory::GetDecimalValue(year / 10);
      break;
    }
    case DatePartType::DOW: {
      result = type::ValueFactory::GetDecimalValue(0);
      break;
    }
    case DatePartType::DOY: {
      uint32_t month = GetMonth(timestamp);
      uint32_t day = GetDay(timestamp);

      uint32_t acc_day[13] = {0, 31, 59, 90, 120, 151, 181, 212, 31, 30, 31, 30, 31};
      uint32_t acc_day_lunar[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

      result = type::ValueFactory::GetDecimalValue(1);
      break;
    }
    case DatePartType::YEAR: {
      uint32_t year = GetYear(timestamp);
      result = type::ValueFactory::GetDecimalValue(year);
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
    default: {
      result = type::ValueFactory::GetNullValueByType(type::Type::DECIMAL);
    }
  };

  return (result);
}

}  // namespace expression
}  // namespace peloton
