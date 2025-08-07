#pragma once

#ifndef _AR_INTERNAL_LOG_MACRO_
#define _AR_INTERNAL_LOG_MACRO_

#include <source_location>

#define AR_LOG(CategoryName, Verbosity, Format, ...) \
  { \
    FString msg = FString::Printf(Format, ##__VA_ARGS__); \
    static std::source_location src_loc = std::source_location::current(); \
    msg += FString::Printf(TEXT(" Function: [%hs]"), src_loc.function_name()); \
    UE_LOG(CategoryName, Verbosity, TEXT("%s"), *msg); \
  }

#endif // _AR_INTERNAL_LOG_MACRO_
