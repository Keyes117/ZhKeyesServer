

#include <iostream>

#include "Calc24Server.h"

int main()
{
    Calc24Server calc24Serevr;
    if (!calc24Serevr.init(5, "0.0.0.0", 8888))
    {
        return 0;
    }
    return 1;
}