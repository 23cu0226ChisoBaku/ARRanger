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

#ifndef _MLIBRARY_ZEROMEMORY_
#define _MLIBRARY_ZEROMEMORY_

#define ZeroMemory(src,size)      \
        {                         \
          memset(src, 0, size);   \
        }

#define ZeroMemory_Class()  ZeroMemory(this, sizeof(*this))
        
#endif // _MLIBRARY_ZEROMEMORY_


