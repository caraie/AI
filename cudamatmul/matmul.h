#ifndef MATMUL_H
#define MATMUL_H

/*
 * Square matrix product, row major and single precision.
 *
 * Everything is float on purpose: double precision only exists on the
 * newest GT200 boards and even there it runs at a fraction of the single
 * precision rate, so for this card there is no point.
 */

void matmulHost(const float *a, const float *b, float *c, int n);

/*
 * Same product on the card. Allocates, copies, runs the kernel and copies
 * the result back. Returns the time the kernel took in milliseconds, or
 * -1 when there is no device to run on.
 */
float matmulDevice(const float *a, const float *b, float *c, int n);

/* Naive kernel, one thread per element and everything read from global
   memory. Kept around to have something to compare the tiled version
   against. */
float matmulDeviceNaive(const float *a, const float *b, float *c, int n);

/* Same generator on host and device so both sides see the same numbers. */
void fillMatrix(float *m, int count, unsigned long seed);

float maxDifference(const float *a, const float *b, int count);

#endif
