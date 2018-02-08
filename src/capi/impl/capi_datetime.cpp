#include <capi/TuriCore.h>

#include <memory>

#include <export.hpp>
#include <capi/impl/capi_error_handling.hpp>
#include <capi/impl/capi_wrapper_structs.hpp>

/******************************************************************************/
/*                                                                            */
/*    flex_datetime                                                           */
/*                                                                            */
/******************************************************************************/

extern "C" {

EXPORT tc_datetime* tc_datetime_create_empty(tc_error** error) {
  ERROR_HANDLE_START();

  return new_tc_datetime();

  ERROR_HANDLE_END(error, nullptr);
}

// Create and set a datetime object from a posix timestamp value --
// the number of seconds since January 1, 1970, UTC.
EXPORT tc_datetime* tc_datetime_create_from_posix_timestamp(
  int64_t posix_timestamp, tc_error** error) {
  ERROR_HANDLE_START();

  return new_tc_datetime(posix_timestamp);

  ERROR_HANDLE_END(error, nullptr);
}

// Create and set a datetime object from a high res posix timestamp value --
// the number of seconds since January 1, 1970, UTC, in double precision.
EXPORT tc_datetime* tc_datetime_create_from_posix_highres_timestamp(
    double posix_timestamp, tc_error** error) {
  ERROR_HANDLE_START();

  turi::flex_date_time impl;
  impl.set_microsecond_res_timestamp(posix_timestamp);
  return new_tc_datetime(std::move(impl));

  ERROR_HANDLE_END(error, nullptr);
}

// Set the datetime value from a string timestamp of the date and/or time.
EXPORT tc_datetime* tc_datetime_create_from_string(
      const char* datetime_str, const char* format_str, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, datetime_str, "Datetime string", nullptr);

  std::string format;
  if (format_str != nullptr) {
    format = format_str;
  }
  turi::flexible_type_impl::date_time_string_reader reader(std::move(format));

  return new_tc_datetime(reader.read(datetime_str));

  ERROR_HANDLE_END(error, nullptr);
}

// Set and get the time zone.  The time zone has 15 min resolution.
EXPORT void tc_datetime_set_time_zone_offset(
    tc_datetime* dt, int64_t n_tz_hour_offset, int64_t n_tz_15min_offsets,
    tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime");

  get_value(dt).set_time_zone_offset(4* n_tz_hour_offset + n_tz_15min_offsets);

  ERROR_HANDLE_END(error);
}
EXPORT int64_t tc_datetime_get_time_zone_offset_minutes(
    const tc_datetime* dt, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime", 0);

  return get_value(dt).time_zone_offset() * 15;

  ERROR_HANDLE_END(error, 0);
}

// Set and get the microsecond part of the time zone.
EXPORT void tc_datetime_set_microsecond(
      tc_datetime* dt, uint64_t microseconds, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime");

  get_value(dt).set_microsecond(microseconds);

  ERROR_HANDLE_END(error);
}
EXPORT uint64_t tc_datetime_get_microsecond(
      const tc_datetime* dt, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime", 0);

  return get_value(dt).microsecond();

  ERROR_HANDLE_END(error, 0);
}

// Set and get the posix style timestamp -- number of seconds since January 1,
// 1970, UTC.
EXPORT void tc_datetime_set_timestamp(
      tc_datetime* dt, int64_t d, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime");

  get_value(dt).set_posix_timestamp(d);

  ERROR_HANDLE_END(error);
}
EXPORT int64_t tc_datetime_get_timestamp(tc_datetime* dt, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime", 0);

  return get_value(dt).posix_timestamp();

  ERROR_HANDLE_END(error, 0);
}

// Set and get the posix style timestamp with high res counter -- number of
// seconds since January 1, 1970, UTC.
EXPORT void tc_datetime_set_highres_timestamp(
    tc_datetime* dt, double d, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime");

  get_value(dt).set_microsecond_res_timestamp(d);

  ERROR_HANDLE_END(error);
}
EXPORT double tc_datetime_get_highres_timestamp(
    tc_datetime* dt, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt, "Datetime", 0.0);

  return get_value(dt).microsecond_res_timestamp();

  ERROR_HANDLE_END(error, 0.0);
}

// Returns nonzero if the time dt1 is before the time dt2
EXPORT int tc_datetime_less_than(
      const tc_datetime* dt1, const tc_datetime* dt2, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt1, "Datetime", false);
  CHECK_NOT_NULL(error, dt2, "Datetime", false);

  return get_value(dt1) < get_value(dt2);

  ERROR_HANDLE_END(error, 0);
}

// Returns nonzero if the time dt1 is equal to the time dt2
EXPORT int tc_datetime_equal(
      const tc_datetime* dt1, const tc_datetime* dt2, tc_error** error) {
  ERROR_HANDLE_START();

  CHECK_NOT_NULL(error, dt1, "Datetime", false);
  CHECK_NOT_NULL(error, dt2, "Datetime", false);

  return get_value(dt1) == get_value(dt2);

  ERROR_HANDLE_END(error, 0);
}

// Destructor
EXPORT void tc_datetime_destroy(tc_datetime* dt) {
  delete dt->impl;
}

}  // extern "C"
