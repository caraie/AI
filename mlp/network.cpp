#include "network.h"

#include <cmath>
#include <cstddef>

double sigmoid(double x)
{
    return 1.0 / (1.0 + std::exp(-x));
}

Network::Network(int inputs, int hidden, int outputs, Random &random)
    : m_inputs(inputs), m_hidden(hidden), m_outputs(outputs),
      m_hiddenValues(hidden, 0.0), m_outputValues(outputs, 0.0)
{
    /* Small random weights around zero. All zeros would leave every hidden
       unit computing the same thing forever. */
    m_hiddenWeights.resize(hidden);
    for (int h = 0; h < hidden; ++h)
    {
        m_hiddenWeights[h].resize(inputs + 1);
        for (int i = 0; i <= inputs; ++i)
            m_hiddenWeights[h][i] = random.between(-1.0, 1.0);
    }

    m_outputWeights.resize(outputs);
    for (int o = 0; o < outputs; ++o)
    {
        m_outputWeights[o].resize(hidden + 1);
        for (int h = 0; h <= hidden; ++h)
            m_outputWeights[o][h] = random.between(-1.0, 1.0);
    }
}

const std::vector<double> &Network::forward(const std::vector<double> &input)
{
    for (int h = 0; h < m_hidden; ++h)
    {
        double sum = m_hiddenWeights[h][m_inputs]; /* bias */
        for (int i = 0; i < m_inputs; ++i)
            sum += m_hiddenWeights[h][i] * input[i];
        m_hiddenValues[h] = sigmoid(sum);
    }

    for (int o = 0; o < m_outputs; ++o)
    {
        double sum = m_outputWeights[o][m_hidden]; /* bias */
        for (int h = 0; h < m_hidden; ++h)
            sum += m_outputWeights[o][h] * m_hiddenValues[h];
        m_outputValues[o] = sigmoid(sum);
    }

    return m_outputValues;
}

double Network::learn(const std::vector<double> &input,
                      const std::vector<double> &target,
                      double rate)
{
    forward(input);

    double error = 0.0;
    std::vector<double> outputDelta(m_outputs);

    for (int o = 0; o < m_outputs; ++o)
    {
        double difference = target[o] - m_outputValues[o];
        error += difference * difference;
        /* derivative of the logistic is y * (1 - y) */
        outputDelta[o] = difference * m_outputValues[o] * (1.0 - m_outputValues[o]);
    }

    std::vector<double> hiddenDelta(m_hidden);
    for (int h = 0; h < m_hidden; ++h)
    {
        double sum = 0.0;
        for (int o = 0; o < m_outputs; ++o)
            sum += outputDelta[o] * m_outputWeights[o][h];
        hiddenDelta[h] = sum * m_hiddenValues[h] * (1.0 - m_hiddenValues[h]);
    }

    /* update the output layer first, it still needs the old hidden weights
       above, not below */
    for (int o = 0; o < m_outputs; ++o)
    {
        for (int h = 0; h < m_hidden; ++h)
            m_outputWeights[o][h] += rate * outputDelta[o] * m_hiddenValues[h];
        m_outputWeights[o][m_hidden] += rate * outputDelta[o];
    }

    for (int h = 0; h < m_hidden; ++h)
    {
        for (int i = 0; i < m_inputs; ++i)
            m_hiddenWeights[h][i] += rate * hiddenDelta[h] * input[i];
        m_hiddenWeights[h][m_inputs] += rate * hiddenDelta[h];
    }

    return error;
}

double Network::train(const std::vector<std::vector<double> > &inputs,
                      const std::vector<std::vector<double> > &targets,
                      int epochs, double rate)
{
    double error = 0.0;

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        error = 0.0;
        for (std::size_t i = 0; i < inputs.size(); ++i)
            error += learn(inputs[i], targets[i], rate);
    }

    return error;
}
