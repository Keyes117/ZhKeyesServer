#pragma once

#include "util.h"

//手动使程序崩溃
void crash()
{
    char* p = nullptr;
    *p = 0;
}