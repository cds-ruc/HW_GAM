//
// Created by lianyu on 2023/1/6.
//


#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)

#pragma GCC diagnostic ignored "-Wattributes"

#ifdef MCS_STATIC
#define MCS_EXPORT
#elif defined(MCS_EXPORTING)
#define MCS_EXPORT __declspec(dllexport)
#else
#define MCS_EXPORT __declspec(dllimport)
#endif

#define MCS_NO_EXPORT
#else
#ifndef MCS_EXPORT
#define MCS_EXPORT __attribute__((visibility("default")))
#endif
#ifndef MCS_NO_EXPORT
#define MCS_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif


#if defined(__clang__)
#define MCS_EXTERN_TEMPLATE extern template class MCS_EXPORT
#else
#define MCS_EXTERN_TEMPLATE extern template class
#endif


#if defined(_MSC_VER) || defined(__clang__)
#define MCS_TEMPLATE_EXPORT MCS_EXPORT
#else
#define MCS_TEMPLATE_EXPORT
#endif

