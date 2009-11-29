#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "matmul.h"

namespace {

/* The gpu tests are skipped when there is no card in the machine, so the
   same binary still says something useful on the laptop. */
bool deviceAvailable = false;

void testHostProductByHand()
{
    /* [1 2; 3 4] * [5 6; 7 8] = [19 22; 43 50] */
    float a[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float b[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float c[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    matmulHost(a, b, c, 2);

    assert(c[0] == 19.0f);
    assert(c[1] == 22.0f);
    assert(c[2] == 43.0f);
    assert(c[3] == 50.0f);
}

void testHostIdentity()
{
    const int n = 8;
    std::vector<float> a(n * n), identity(n * n, 0.0f), c(n * n);

    fillMatrix(&a[0], n * n, 1);
    for (int i = 0; i < n; ++i)
        identity[i * n + i] = 1.0f;

    matmulHost(&a[0], &identity[0], &c[0], n);
    assert(maxDifference(&a[0], &c[0], n * n) == 0.0f);
}

void testFillIsRepeatable()
{
    std::vector<float> first(64), second(64), other(64);

    fillMatrix(&first[0], 64, 7);
    fillMatrix(&second[0], 64, 7);
    fillMatrix(&other[0], 64, 8);

    assert(maxDifference(&first[0], &second[0], 64) == 0.0f);
    assert(maxDifference(&first[0], &other[0], 64) > 0.0f);

    for (int i = 0; i < 64; ++i)
        assert(first[i] >= -0.5f && first[i] <= 0.5f);
}

void testMaxDifference()
{
    float a[3] = {1.0f, 2.0f, 3.0f};
    float b[3] = {1.0f, 2.5f, 3.0f};

    assert(maxDifference(a, a, 3) == 0.0f);
    assert(std::fabs(maxDifference(a, b, 3) - 0.5f) < 1e-6f);
}

/* Sizes that are a multiple of the tile and sizes that are not, the
   second kind is where the bounds checks earn their keep. */
void testDeviceMatchesHost()
{
    if (!deviceAvailable) return;

    const int sizes[] = {1, 15, 16, 17, 64, 129};

    for (int s = 0; s < 6; ++s)
    {
        int n = sizes[s];
        std::vector<float> a(n * n), b(n * n), fromHost(n * n), fromDevice(n * n);

        fillMatrix(&a[0], n * n, 11 + s);
        fillMatrix(&b[0], n * n, 97 + s);

        matmulHost(&a[0], &b[0], &fromHost[0], n);
        assert(matmulDevice(&a[0], &b[0], &fromDevice[0], n) >= 0.0f);

        /* single precision, so the sums do not come out bit for bit */
        assert(maxDifference(&fromHost[0], &fromDevice[0], n * n) < 1e-3f);
    }
}

void testTiledAgreesWithNaive()
{
    if (!deviceAvailable) return;

    const int n = 96;
    std::vector<float> a(n * n), b(n * n), tiled(n * n), naive(n * n);

    fillMatrix(&a[0], n * n, 3);
    fillMatrix(&b[0], n * n, 5);

    assert(matmulDevice(&a[0], &b[0], &tiled[0], n) >= 0.0f);
    assert(matmulDeviceNaive(&a[0], &b[0], &naive[0], n) >= 0.0f);

    assert(maxDifference(&tiled[0], &naive[0], n * n) < 1e-3f);
}

void testEmptyMatrix()
{
    float dummy = 0.0f;
    assert(matmulDevice(&dummy, &dummy, &dummy, 0) < 0.0f);
}

}

int main()
{
    float probe = 0.0f;
    deviceAvailable = matmulDevice(&probe, &probe, &probe, 1) >= 0.0f;

    testHostProductByHand();
    testHostIdentity();
    testFillIsRepeatable();
    testMaxDifference();
    testEmptyMatrix();
    testDeviceMatchesHost();
    testTiledAgreesWithNaive();

    if (!deviceAvailable)
    {
        printf("no cuda device here, only the host tests ran\n");
        printf("all tests passed\n");
        return 0;
    }

    const int n = 512;
    std::vector<float> a(n * n), b(n * n), c(n * n);
    fillMatrix(&a[0], n * n, 2009);
    fillMatrix(&b[0], n * n, 1979);

    float naive = matmulDeviceNaive(&a[0], &b[0], &c[0], n);
    float tiled = matmulDevice(&a[0], &b[0], &c[0], n);

    printf("%d x %d: naive %.2f ms, tiled %.2f ms, %.1fx\n",
           n, n, naive, tiled, naive / tiled);
    printf("all tests passed\n");
    return 0;
}
