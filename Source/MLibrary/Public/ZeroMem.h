// Fill out your copyright notice in the Description page of Project Settings.
/*

ZeroMem.h

Author : MAI ZHICONG

Description : ZeroMemory Macros

Update History: 2024/12/16 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_LIB_ZEROMEMORY
#define M_LIB_ZEROMEMORY

#define ZeroMemory(src,size)      \
        {                         \
          memset(src, 0, size);   \
        }

#define ZeroMemory_Class()  ZeroMemory(this, sizeof(this))
        
#endif

