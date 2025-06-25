// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef MLIB_CPP_SUPPORT
#define MLIB_CPP_SUPPORT

#if defined(__clang__) || defined(__GNUC__)
  #define CPP_STANDARD __cpluspluc
#elif defined(_MSC_VER)
  #define CPP_STANDARD _MSVC_LANG
#endif

#define HAS_CPP11 CPP_STANDARD >= 201103L
#define HAS_CPP14 CPP_STANDARD >= 201402L
#define HAS_CPP17 CPP_STANDARD >= 201703L
#define HAS_CPP20 CPP_STANDARD >= 202002L
#define HAS_CPP23 CPP_STANDARD >= 202302L

#endif