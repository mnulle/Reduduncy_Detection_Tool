/***************************************************************
 * File: polynomial.h
 * Author: Matt Nulle
 * This header declares several functions necessary to 
 *   perform the "polynomial mod polynomial" op
 **************************************************************/

#ifndef POLYNOMIAL_REDUCTION
#define POLYNOMIAL_REDUCTION

#include <stdbool.h>
#include <stdint.h>

/**
 * args: 
 *   uint32_t* f: representation of the left polynomial in the operation
 *   uint32_t* p: representation of the right polynomial in the operation
 *   int len         : length of array f
 *   int plen        : length of array p
 * return:
 *   pointer to f, where the first plen elements of f now contain the output
 */
uint32_t* poly_mod(uint32_t* f, uint32_t* p, int len, int plen);

/** 
 * args:
 *   uint32_t* f: representation of the left polynomial in the operation
 *   int len         : length of the array f
 *   int plen        : length of the array p
 * return:
 *   returns true if the value in f is still too large, based on plen
 */
bool keep_going(uint32_t* f, int len, int plen);

/**
 * args:
 *   uint32_t* f: the array to shift
 *   int len         : length of the array f
 * return:
 *   returns pointer to f, which has been shifted left by 1 bit
 */
uint32_t* shift_array(uint32_t* f, int len);

#endif
