/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    stringhelper.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stringhelper.h>

std::string to_string(int n) {
    std::string s;

    if(n==0)
    {
        s="0";
        return s;
    }

    int sign = -(n<0);
    unsigned int val = (n^sign)-sign;

    int size;
    if(val>=10000)
    {
        if(val>=10000000)
        {
            if(val>=1000000000)
                size=10;
            else if(val>=100000000)
                size=9;
            else
                size=8;
        }
        else
        {
            if(val>=1000000)
                size=7;
            else if(val>=100000)
                size=6;
            else
                size=5;
        }
    }
    else
    {
        if(val>=100)
        {
            if(val>=1000)
                size=4;
            else
                size=3;
        }
        else
        {
            if(val>=10)
                size=2;
            else
                size=1;
        }
    }

    s.resize(-sign+size);
    char* c = &s[0];
    if(sign)
        *c++='-';

    char* d = c+size-1;
    while(val>0)
    {
        *d--='0' + (val % 10);
        val /= 10;
    }
    return s;
}

std::string to_string(unsigned val ) {

    std::string s;

    if(val==0)
    {
        s="0";
        return s;
    }

    int size;
    if(val>=10000)
    {
        if(val>=10000000)
        {
            if(val>=1000000000)
                size=10;
            else if(val>=100000000)
                size=9;
            else
                size=8;
        }
        else
        {
            if(val>=1000000)
                size=7;
            else if(val>=100000)
                size=6;
            else
                size=5;
        }
    }
    else
    {
        if(val>=100)
        {
            if(val>=1000)
                size=4;
            else
                size=3;
        }
        else
        {
            if(val>=10)
                size=2;
            else
                size=1;
        }
    }

    s.resize(size);
    char* c = &s[0];
    char* d = c+size-1;
    while(val>0)
    {
        *d--='0' + (val % 10);
        val /= 10;
    }
    return s;
}

