//
// Created by lianyu on 2023/1/6.
//

#pragma once

// From Google gutil
#ifndef MCS_DISALLOW_COPY_AND_ASSIGN
#define MCS_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &) = delete;         \
  void operator=(const TypeName &) = delete
#endif

#define MCS_UNUSED(x) (void)x

//
// GCC can be told that a certain branch is not likely to be taken (for
// instance, a CHECK failure), and use that information in static analysis.
// Giving it this information can help it optimize for the common case in
// the absence of better information (ie. -fprofile-arcs).
//
#if defined(__GNUC__)
#define MCS_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define MCS_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#define MCS_NORETURN __attribute__((noreturn))
#define MCS_PREFETCH(addr) __builtin_prefetch(addr)
#elif defined(_MSC_VER)
#define MCS_NORETURN __declspec(noreturn)
#define MCS_PREDICT_FALSE(x) x
#define MCS_PREDICT_TRUE(x) x
#define MCS_PREFETCH(addr)
#else
#define MCS_NORETURN
#define MCS_PREDICT_FALSE(x) x
#define MCS_PREDICT_TRUE(x) x
#define MCS_PREFETCH(addr)
#endif

#if (defined(__GNUC__) || defined(__APPLE__))
#define MCS_MUST_USE_RESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#define MCS_MUST_USE_RESULT
#else
#define MCS_MUST_USE_RESULT
#endif

// Suppress Undefined Behavior Sanitizer (recoverable only). Usage:
// - __suppress_ubsan__("undefined")
// - __suppress_ubsan__("signed-integer-overflow")
// adaped from
// https://github.com/google/flatbuffers/blob/master/include/flatbuffers/base.h
#if defined(__clang__)
#define __suppress_ubsan__(type) __attribute__((no_sanitize(type)))
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 409)
#define __suppress_ubsan__(type) __attribute__((no_sanitize_undefined))
#else
#define __suppress_ubsan__(type)
#endif
