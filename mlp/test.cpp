#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "network.h"
#include "random.h"

namespace {

std::vector<double> pair(double a, double b)
{
    std::vector<double> v;
    v.push_back(a);
    v.push_back(b);
    return v;
}

std::vector<double> one(double a)
{
    std::vector<double> v;
    v.push_back(a);
    return v;
}

void xorSet(std::vector<std::vector<double> > &inputs,
            std::vector<std::vector<double> > &targets)
{
    inputs.push_back(pair(0, 0)); targets.push_back(one(0));
    inputs.push_back(pair(0, 1)); targets.push_back(one(1));
    inputs.push_back(pair(1, 0)); targets.push_back(one(1));
    inputs.push_back(pair(1, 1)); targets.push_back(one(0));
}

void testSigmoid()
{
    assert(std::fabs(sigmoid(0.0) - 0.5) < 1e-12);
    assert(sigmoid(10.0) > 0.99);
    assert(sigmoid(-10.0) < 0.01);
}

void testRandomIsRepeatable()
{
    Random a(7), b(7);
    for (int i = 0; i < 100; ++i)
        assert(a.next() == b.next());

    Random c(1);
    for (int i = 0; i < 1000; ++i)
    {
        double value = c.uniform();
        assert(value >= 0.0 && value < 1.0);
    }
}

void testUntrainedOutputIsInRange()
{
    Random random(3);
    Network net(2, 3, 1, random);

    const std::vector<double> &output = net.forward(pair(1, 0));
    assert(output.size() == 1);
    assert(output[0] > 0.0 && output[0] < 1.0);
}

void testErrorGoesDown()
{
    Random random(11);
    Network net(2, 3, 1, random);

    std::vector<std::vector<double> > inputs, targets;
    xorSet(inputs, targets);

    double first = net.train(inputs, targets, 1, 0.5);
    double later = net.train(inputs, targets, 2000, 0.5);
    assert(later < first);
}

void testLearnsXor()
{
    Random random(11);
    Network net(2, 3, 1, random);

    std::vector<std::vector<double> > inputs, targets;
    xorSet(inputs, targets);

    net.train(inputs, targets, 20000, 0.5);

    for (std::size_t i = 0; i < inputs.size(); ++i)
    {
        double output = net.forward(inputs[i])[0];
        double wanted = targets[i][0];
        assert(std::fabs(output - wanted) < 0.1);
    }
}

}

int main()
{
    testSigmoid();
    testRandomIsRepeatable();
    testUntrainedOutputIsInRange();
    testErrorGoesDown();
    testLearnsXor();

    Random random(11);
    Network net(2, 3, 1, random);
    std::vector<std::vector<double> > inputs, targets;
    xorSet(inputs, targets);
    double error = net.train(inputs, targets, 20000, 0.5);
    printf("error after 20000 epochs: %.6f\n", error);
    for (std::size_t i = 0; i < inputs.size(); ++i)
        printf("%g xor %g = %.3f\n", inputs[i][0], inputs[i][1],
               net.forward(inputs[i])[0]);
    printf("all tests passed\n");
    return 0;
}
