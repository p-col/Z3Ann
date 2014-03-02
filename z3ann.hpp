
#ifndef Z3ANN_HPP_
# define Z3ANN_HPP_

# include <vector>
# include <string>
# include "z3++.h"
# include "Z3SolutionConverter.hpp"

# define DEFAULTPRECISION 10000
# define TOINT(x, precision) ((int)(x * precision))

class LinkDescriptor
{
private:
    int _sizeEdge;

public:

    std::vector<std::vector<bool> > _linkTable;

    LinkDescriptor(int sizeEdge):
    _sizeEdge(sizeEdge),
    _linkTable(sizeEdge)
    {
        int i;

        for (i = 0; i < _sizeEdge; ++i)
            _linkTable[i].resize(sizeEdge);
        setAll(true);
    }

    void setAll(bool enabled)
    {
        int i;
        int j;

        for (i = 0; i < _sizeEdge; ++i)
            for (j = 0; j < _sizeEdge; ++j)
                _linkTable[i][j] = enabled;
    }

    bool getLink(int from, int to) const
    {
        if (from >= _sizeEdge || to >= _sizeEdge)
            return false;
        return _linkTable[from][to];
    }

    bool setLink(int from, int to, bool enabled)
    {
        if (from >= _sizeEdge || to >= _sizeEdge)
            return false;


        _linkTable[from][to] = enabled;
        return true;
    }

    int getSizeEdge(void) const {return _sizeEdge;}
};

// FIXME :: add majuscule to the name
class trainingData
{
public:
    std::vector<float> input;
    std::vector<float> output;

    trainingData()
    {
    }

    trainingData(const trainingData &cpy)
    {
        *this = cpy;
    }

    trainingData &operator=(const trainingData &cpy)
    {
        size_t i;

        input.resize(cpy.input.size());
        output.resize(cpy.output.size());

        for (i = 0; i < cpy.input.size(); ++i)
            input[i] = cpy.input[i];
        for (i = 0; i < cpy.output.size(); ++i)
            output[i] = cpy.output[i];
        return *this;
    }
};

typedef struct 
{
    std::vector<z3::expr*> input;
    std::vector<z3::expr*> output;
} z3trainingData;


class Z3Equation
{

private:
    std::vector<z3::expr*> _inputNodes;
    std::vector<z3::expr*> _outputNodes;
    std::vector<z3::expr*> _hiddenNodes;

    trainingData _trainingData;

    int _id;
    int _precision;
    z3::context &_context;
    float _min;
    float _max;
    bool _isInitialized;


public:

    static const float E;

    float getMin(void) const {return _min;}
    float getMax(void) const {return _max;}


    Z3Equation(int inputSize, int hiddenSize, int outputSize, int id,
               z3::context &context, float min, float max,
               int precision = DEFAULTPRECISION) :
        _inputNodes(inputSize, NULL),
        _outputNodes(outputSize, NULL),
        _hiddenNodes(hiddenSize, NULL),
        _id(id),
        _precision(precision),
        _context(context),
        _min(min),
        _max(max),
        _isInitialized(false)
    {
        int i;
        std::string str;
        std::stringstream ss;

        for (i = 0; i < inputSize; ++i) {
            str.clear();
            str += "i";
            ss.clear();
            ss.str(std::string());
            ss << i;
            str += ss.str();
            str += "_";
            ss.clear();
            ss.str(std::string());
            ss << _id;
            str += ss.str();
            // std::cout << "new input:" << str << std::endl;
            _inputNodes[i] = new z3::expr(_context.real_const(str.c_str()));
        }

        for (i = 0; i < hiddenSize; ++i) {
            str.clear();
            str += "h";
            ss.clear();
            ss.str(std::string());
            ss << i;
            str += ss.str();
            str += "_";
            ss.clear();
            ss.str(std::string());
            ss << _id;
            str += ss.str();
            _hiddenNodes[i] = new z3::expr(_context.real_const(str.c_str()));
        }

        for (i = 0; i < outputSize; ++i) {
            str.clear();
            str += "o";
            ss.clear();
            ss.str(std::string());
            ss << i;
            str += ss.str();
            str += "_";
            ss.clear();
            ss.str(std::string());
            ss << _id;
            str += ss.str();
            _outputNodes[i] = new z3::expr(_context.real_const(str.c_str()));
        }
    }

    virtual ~Z3Equation()
    {
        std::vector<z3::expr*>::iterator it;

        for (it = _inputNodes.begin(); it != _inputNodes.end(); ++it)
            delete *it;
        for (it = _outputNodes.begin(); it != _outputNodes.end(); ++it)
            delete *it;
        for (it = _hiddenNodes.begin(); it != _hiddenNodes.end(); ++it)
            delete *it;
    }

    bool init(const trainingData &data);

    bool assignInpoutOutputValues(void)
    {
        std::vector<float>::const_iterator it;
        size_t i;
        
        // FIXME : check for error outside of the loops
        for (it = _trainingData.input.begin(), i = 0;
             it != _trainingData.input.end();
             ++it, ++i)
            {
                if (i >= _inputNodes.size())
                    {
                        std::cerr << "Error : Bad input _trainingData (array too long)" <<
                            std::endl;
                        return false;
                    }
                *(_inputNodes[i]) = (*(_inputNodes[i])) == TOINT(*it, _precision);
            }

        for (it = _trainingData.output.begin(), i = 0;
             it != _trainingData.output.end();
             ++it, ++i)
            {
                if (i >= _outputNodes.size())
                    {
                        std::cerr << "Error : Bad output _trainingData (array too long)" <<
                            std::endl;
                        return false;
                    }
                if (*it == _min)
                    *(_outputNodes[i]) = (*(_outputNodes[i])) <= TOINT(_min  + ((_max - _min) / 2), _precision);
                else
                    *(_outputNodes[i]) = (*(_outputNodes[i])) > TOINT(_min  + ((_max - _min) / 2), _precision);
                    
            }
        _isInitialized = true;
        return true;
    }

    // FIXME : put in private
    z3::expr *getNode(size_t i)
    {
        if (i < _inputNodes.size())
            return _inputNodes[i];
        else if (i < _inputNodes.size() + _hiddenNodes.size())
            return _hiddenNodes[i - _inputNodes.size()];
        else // if i is in output
            return _outputNodes[i - _inputNodes.size() - _hiddenNodes.size()];
    }

    bool generateEquations(std::vector<std::vector<z3::expr*> > &weights,
                           const LinkDescriptor &linkTable)
    {
        size_t i;
        size_t j;
        size_t totalSizeNode = _inputNodes.size() + _hiddenNodes.size() +
            _outputNodes.size();

        /* checking error of array size */
        if (weights.size() != totalSizeNode)
            {
                std::cerr << "Error: weight table not corresponding to number of nodes in Z3Equation" << std::endl;
                return false;
            }
        for (i = 0; i < weights.size(); ++i)
            if (weights[i].size() != totalSizeNode)
                {
                    std::cerr << "Error: weight table not corresponding to number of nodes in Z3Equation" << std::endl;
                    return false;
                }

        for (i = 0 ; i < totalSizeNode; ++i)
            {
                // build equation
                z3::expr tmp(_context.real_const("tmp"));
                bool b = true;
                for (j = 0; j < totalSizeNode; ++j)
                    {
                        if (linkTable.getLink(j, i))
                            {
                                if (b)
                                    {
                                        tmp = *(weights[j][i]) * *(getNode(j));
                                        b = false;
                                    }
                                else
                                    tmp = tmp + (*(weights[j][i]) * *(getNode(j)));
                            }

                    }
                // FIXME : improve activation function
                // sigmoid(&tmp) for instance;
                tmp = ite(tmp > TOINT((_min + (_max - _min) / 2.f), _precision),  _context.real_val(TOINT(_max, _precision)),
                          _context.real_val(TOINT(_min, _precision)));
                if (!b)
                    *getNode(i) = tmp;
            }
        return true;
    }

    void addConstraintsToSolver(z3::solver &solver)
    {
        std::vector<z3::expr*>::iterator it;

        for (it = _inputNodes.begin(); it != _inputNodes.end(); ++it)
            {
                (*it)->simplify();
                solver.add(*(*it));
            }
        for (it = _outputNodes.begin(); it != _outputNodes.end(); ++it)
            {
                (*it)->simplify();
                solver.add(*(*it));
            }
        // FIXME : remove because not a condition
        // for (it = _hiddenNodes.begin(); it != _hiddenNodes.end(); ++it)
        //     solver.add(*(*it));
    }

};

class z3ann {

private:
    int _equationId;

    int _inputSize;
    int _hiddenSize;
    int _outputSize;

    int _precision;
    float _min;
    float _max;

    bool _isSolved;

    std::vector<Z3Equation*> _equations;
    std::vector<std::vector<z3::expr*> > _weights;
    std::vector<std::vector<float> > _realWeights;
    LinkDescriptor _linkTable;

    // keep in this order for init list
    z3::context _context;
    z3::solver _solver;


    float &getRealWeightFromName(const std::string &str);

public:

    z3ann(int inputSize, int hiddenSize, int outputSize,
          float min = -1, float max = 1,
          int precision = DEFAULTPRECISION) :
        _equationId(0),
        _inputSize(inputSize),
        _hiddenSize(hiddenSize),
        _outputSize(outputSize),
        _precision(precision),
        _min(min),
        _max(max),
        _isSolved(false),
        _linkTable(inputSize + hiddenSize + outputSize),
        _solver(_context)
    {
        int totalSizeNode = _inputSize + _hiddenSize + _outputSize;
        std::stringstream ss;
        std::string str;
        int i;
        int j;

        _weights.resize(totalSizeNode);
        _realWeights.resize(totalSizeNode);
        for (i = 0; i < totalSizeNode; ++i)
            {
                _weights[i].resize(totalSizeNode);
                _realWeights[i].resize(totalSizeNode);
                for (j = 0; j < totalSizeNode; ++j)
                    {
                        ss.clear();
                        ss.str(std::string());
                        ss << i;
                        ss << "_";
                        ss << j;
                        str = "w";
                        str += ss.str();
                        _weights[i][j] = new z3::expr(_context.real_const(str.c_str()));
                        _realWeights[i][j] = 0.f;
                    }
            }
    }

    // FIXME : remake copy constructor
    z3ann(const z3ann &ann) :
        _equationId(0),
        _inputSize(ann._inputSize),
        _hiddenSize(ann._hiddenSize),
        _outputSize(ann._outputSize),
        _precision(ann._precision),
        _linkTable(_inputSize + _hiddenSize + _outputSize),
        _solver(_context)
    {
        int totalSizeNode = _inputSize + _hiddenSize + _outputSize;
        std::vector<std::vector<z3::expr*> >::iterator it;
        std::vector<z3::expr*>::iterator iit;
        std::stringstream ss;
        std::string str;
        int i;
        int j;

        _weights.resize(totalSizeNode);
        for (it = _weights.begin(), i = 0; it != _weights.end(); ++it, ++i)
            for (iit = it->begin(), j = 0; iit != it->end(); ++iit, ++j)
                {
                    ss.clear();
                    ss << i;
                    ss << "_";
                    ss << j;
                    str = "w";
                    str += ss.str();
                    *iit = new z3::expr(_context.real_const(str.c_str()));
                }

    }

    virtual ~z3ann()
    {
        std::vector<std::vector<z3::expr*> >::iterator wit;
        std::vector<z3::expr*>::iterator wiit;
        std::vector<Z3Equation*>::iterator it;

        for (it = _equations.begin(); it != _equations.end(); ++it)
            delete *it;
        for (wit = _weights.begin(); wit != _weights.end(); ++wit)
            for (wiit = wit->begin(); wiit != wit->end(); ++wiit)
                delete *wiit;
    }

    bool isSolved(void) const {return _isSolved;}
    int getInputSize(void) const {return _inputSize;}
    int getHiddenSize(void) const {return _hiddenSize;}
    int getOutputSize(void) const {return _outputSize;}
    const LinkDescriptor &getLinkTable(void) const {return _linkTable;}
    const std::vector<std::vector<float> > getWeights(void) const {return _realWeights;}
    // FIXME set individual threshold
    float getNodeThreshold(int nodeNum) const {(void)nodeNum; return _min + (_max - _min) / 2.f;}
    // FIXME create individual min and max
    float getNodeMin(int nodeNum) const {(void)nodeNum; return _min;}
    float getNodeMax(int nodeNum) const {(void)nodeNum; return _max;}

    bool loadTrainingData(const trainingData &data);
    bool loadTrainingDataSet(const std::vector<trainingData> &data);
    bool setLink(int from, int to, bool enabled);
    void setAllLink(bool enabled);
    bool solve(void);
};


#endif
