
#ifndef BACKENDC_HPP_
# define BACKENDC_HPP_

#include "z3ann.hpp"

class BackendC
{
private:
    const z3ann &_ann;

    std::string getEquationOfNode(int nodeNumber, const LinkDescriptor &linkTable,
                                  const std::vector<std::vector<float> > &weights);
    std::string getStringEquation(int outputNumber,
                                  const LinkDescriptor &linkTable,
                                  const std::vector<std::vector<float> > &weights);
    std::string generateCode(const LinkDescriptor &linkTable,
                             const std::vector<std::vector<float> > &weights);
public:

    BackendC(const z3ann &ann):
        _ann(ann)
    {}

    virtual ~BackendC()
    {}

    virtual std::string getOutput(void);
};

#endif
