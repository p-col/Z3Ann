
#include "BackendC.hpp"

std::string BackendC::getEquationOfNode(int nodeNumber,
                                        const LinkDescriptor &linkTable,
                                        const std::vector<std::vector<float> > &weights)
    {
        int i;
        std::stringstream str;
        bool b = false;

        if (nodeNumber < _ann.getInputSize())
            str << "input[" << nodeNumber << "]";
        else
            for (i = 0; i < linkTable.getSizeEdge(); ++i)
                {
                    if (linkTable.getLink(i, nodeNumber))
                        {
                            if (b)
                                str << " + ";
                            b = true;
                            str << "(((";
                            str << getEquationOfNode(i, linkTable, weights);
                            str << ")";
                            str << " > " << _ann.getNodeThreshold(nodeNumber) << ".f";
                            str << ") ? " << _ann.getNodeMax(nodeNumber) << ".f";
                            str << " : " << _ann.getNodeMin(nodeNumber) << ".f";
                            str << ")";
                            str << " * ";
                            str << weights[i][nodeNumber];
                        }
                }
        return str.str();
    }

std::string BackendC::getStringEquation(int outputNumber,
                                  const LinkDescriptor &linkTable,
                                  const std::vector<std::vector<float> > &weights)
    {
        return getEquationOfNode(outputNumber + _ann.getHiddenSize() +
                                 _ann.getInputSize(), linkTable, weights);
    }

std::string BackendC::generateCode(const LinkDescriptor &linkTable,
                             const std::vector<std::vector<float> > &weights)
    {
        std::string header = "void propagate(float *input, float *output) {\n";
        std::string footer = "}\n";
        std::stringstream core;
        int i;

        for (i = 0; i < _ann.getOutputSize(); ++i)
            {
                core << "    output[" << i  << "] = ";
                core << getStringEquation(i, linkTable, weights);
                core << ";\n";
            }
        header += core.str();
        header += footer;
        return header;
    }

std::string BackendC::getOutput(void)
    {
        std::string code;

        if (_ann.isSolved())
            {
                const LinkDescriptor &linkTable = _ann.getLinkTable();
                const std::vector<std::vector<float> > &weights = _ann.getWeights();
                code = generateCode(linkTable, weights);
            }
        else
            std::cout << "Error: Coutput: Neural network not solved yet." <<
                std::endl;
        return code;
    }
