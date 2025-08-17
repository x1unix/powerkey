#ifndef H_LOGGER
#define H_LOGGER
#include "config.h"

#ifdef DEBUG
// Helper: recursively print arguments, println on last
template<typename T>
void debug_log_impl(const T &last) {
  Serial.println(last);
}

template<typename T, typename... Args>
void debug_log_impl(const T &first, const Args &...rest) {
  Serial.print(first);
  debug_log_impl(rest...);
}

#define DEBUG_LOG(...) debug_log_impl(__VA_ARGS__)
#else
#define DEBUG_LOG(...) \
  do { \
  } while (0)
#endif

#endif
