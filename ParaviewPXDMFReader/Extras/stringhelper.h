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
// .NAME stringhelper 
// .SECTION Description


#include <sstream>

template<typename T>
std::string to_string( const T & Value ){
    std::ostringstream oss;
    oss << Value;
    return oss.str();
};
;
std::string to_string(int n) ;
//
std::string to_string(unsigned val );
//
template<typename T>
bool from_string( const std::string & Str, T & Dest ){
    std::istringstream iss( Str );
    return iss >> Dest != 0;
};
//
