#ifndef NETWORK_H
#define NETWORK_H

#include <vector>

#include "random.h"

/*
 * Multilayer perceptron with one hidden layer, logistic activation and
 * plain backpropagation. No momentum and no weight decay: the point was
 * to get xor to work, which a single layer cannot do.
 */
class Network
{
public:
    Network(int inputs, int hidden, int outputs, Random &random);

    const std::vector<double> &forward(const std::vector<double> &input);

    /* One backprop step, returns the squared error before the update. */
    double learn(const std::vector<double> &input,
                 const std::vector<double> &target,
                 double rate);

    /* Runs the whole set for the given number of epochs and returns the
       error of the last one. */
    double train(const std::vector<std::vector<double> > &inputs,
                 const std::vector<std::vector<double> > &targets,
                 int epochs, double rate);

private:
    int m_inputs;
    int m_hidden;
    int m_outputs;

    /* weights are [to][from], with the bias kept in an extra input that
       is always 1 */
    std::vector<std::vector<double> > m_hiddenWeights;
    std::vector<std::vector<double> > m_outputWeights;

    std::vector<double> m_hiddenValues;
    std::vector<double> m_outputValues;
};

double sigmoid(double x);

#endif
