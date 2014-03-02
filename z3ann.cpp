
#include <vector>
#include "BackendC.hpp"
#include "z3ann.hpp"

const float Z3Equation::E = 2.71828182846f;

// it's supposed to be no error in this function
float &z3ann::getRealWeightFromName(const std::string &str)
    {
        int y = atoi(&(str[1]));
        int x = 0;
        size_t j;

        j = str.find_first_of('_') + 1;
        x = atoi(&str[j]);

        return _realWeights[y][x];
    }

bool Z3Equation::init(const trainingData &data)
{
    _trainingData = data;
    return true;
}

bool z3ann::loadTrainingData(const trainingData &data)
    {
        Z3Equation *newEquation;
        newEquation = new Z3Equation(_inputSize, _hiddenSize, _outputSize,
                                     ++_equationId, _context, _min, _max,
                                     _precision);
        if (newEquation->init(data))
            {
                _equations.push_back(newEquation);
                return true;
            }
        delete newEquation;
        return false;
    }

bool z3ann::loadTrainingDataSet(const std::vector<trainingData> &data)
    {
        std::vector<trainingData>::const_iterator it = data.begin();
        std::vector<trainingData>::const_iterator end = data.end();

        // _equations.resize(data.size());
        for (; it != end; ++it)
            if (!loadTrainingData(*it))
                return false;
        return true;
    }

bool z3ann::setLink(int from, int to, bool enabled)
    {
        return _linkTable.setLink(from, to, enabled);
    }

    void z3ann::setAllLink(bool enabled)
    {
        _linkTable.setAll(enabled);
    }

    bool z3ann::solve(void)
    {
        std::vector<Z3Equation*>::iterator it;
        bool retVal;

        for (it = _equations.begin(); it != _equations.end(); ++it)
            {
                (*it)->generateEquations(_weights, _linkTable);// get equation
                (*it)->assignInpoutOutputValues();
                (*it)->addConstraintsToSolver(_solver);
            }
        if ((retVal = (_solver.check() == z3::sat)) == true)
            {
                std::cout << "=====================================" << std::endl;
                std::cout << "============== SOLUTION =============" << std::endl;
                std::cout << "=========== (on std error) ==========" << std::endl;
                z3::model m = _solver.get_model();
                for (unsigned i = 0; i < m.size(); i++) {
                    z3::func_decl v = m[i];
                    // this problem contains only constants
                    assert(v.arity() == 0); 
                    // std::cout << v.name().str() << "=" << m.get_const_interp(v)  << std::endl;

                    if (v.name().str()[0] == 'w')
                        {
                            getRealWeightFromName(v.name().str()) = Z3SolutionConverter::getFloatFromZ3Expr(m.get_const_interp(v));
                            std::cerr << v.name() << " = " <<
                                m.get_const_interp(v) << " = \033[1;32m" <<
                                Z3SolutionConverter::getFloatFromZ3Expr(m.get_const_interp(v)) << "\033[0m" << std::endl;
                        }
                }
                // std::cout << _solver.statistics() << std::endl;
                _isSolved = true;
            }
        else
            {
                _isSolved = false;
                std::cout << "Model unsat =(" << std::endl;
            }

        return retVal;
        // std::cout << _solver.get_model() << std::endl;
    }
