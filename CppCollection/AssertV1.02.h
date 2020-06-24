
/**
 * @file
 * @brief Assertion of errors
 * @version 1.02
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 12, 2012-Dec 15, 2012
 * @copyright None.
*/

// #ifndef _ASSERT_H
// #define _ASSERT_H

#include <sstream>
#include <string>

//! throw error message if \c val is not true
#define Assert(val,msg)      \
  if (!(val)) {       \
    throw(msg); \
  } else       \
    ((void) 0)

#if defined(NDEBUG)
//! throw error message if \c val is not true, when NDEBUG is defined
#  define Assert_Dbg(val,msg) ((void) 0)
#else
//! throw error message if \c val is not true, when NDEBUG is defined
#  define Assert_Dbg(val,msg) Assert(val,msg)
#endif

// #endif // #ifndef _ASSERT_H
