#ifndef _DATA_H_
#define _DATA_H_

#include <inttypes.h>

#define BIT(x) uint8_t x:1

// TODO: add type for checkings and castings
typedef union accdata {
    uint64_t u;
    double r;
} data_t; // what can the accumulator be

uint64_t operate_u(unsigned char op, uint64_t a, uint64_t b);
  double operate_d(unsigned char op, double a, double b);
   float operate_f(unsigned char op, float a, float b);

/**
 * @brief operate operator op of type t on data a and b
 * @param operator
 * @param type
 * @param a
 * @param b
 * @return result if available
 */
data_t operate(unsigned char op, unsigned char t, const data_t a, const data_t b);

#endif //_DATA_H_
