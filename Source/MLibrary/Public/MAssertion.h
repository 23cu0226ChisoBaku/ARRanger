// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef MLIB_ASSERTION
#define MLIB_ASSERTION

#include <cassert>

#define not_implemented() \
{ \
  static_assert(false, "Not implemented"); \
}


#define not_implemented_return() \
    not_implemented() \
    return 0;

#endif