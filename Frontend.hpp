
#ifndef FRONTEND_HPP_
# define FRONTEND_HPP_

# include <string>
# include <vector>
# include <fstream>
# include <iostream>
// # include <>

#include "z3ann.hpp"

class Frontend
{
private:

    std::vector<trainingData> _data;

public:

    Frontend(){};
    Frontend(const std::string &path) {loadDataFromFile(path);};
    virtual ~Frontend(){}


    void clear(void) {_data.clear();}
    const std::vector<trainingData> &getData(void) const {return _data;}


    bool loadDataFromFile(const std::string &path);
};

#endif
