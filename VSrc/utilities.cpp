/*
 * utilities.cpp
 *
 *  Created on: October 5, 2022
 *      Author: C Arena
 */

#include "utilities.h"

int _abs_(int x)
{
  if (x < 0)
    x = -x;

  return x;
}

int8_t _min_( int8_t x, int8_t y )
{
    return ((x<y)?x:y);
}

int8_t _max_( int8_t x, int8_t y )
{
    return ((x>y)?x:y);
}

int32_t _min_( int32_t x, int32_t y )
{
    return ((x<y)?x:y);
}

int32_t _max_( int32_t x, int32_t y )
{
    return ((x>y)?x:y);
}
