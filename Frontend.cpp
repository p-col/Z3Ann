
#include "Frontend.hpp"

bool Frontend::loadDataFromFile(const std::string &path)
{
    std::ifstream file(path.c_str());
    int nbSet;
    int inputSize;
    int outputSize;
    int i;
    int j;
    trainingData data;

    if (!file.is_open())
        return false;
    file >> nbSet;
    file >> inputSize;
    file >> outputSize;

    for (i = 0; i < nbSet; ++i)
        {
            data.input.resize(inputSize);
            for (j = 0; j < inputSize; ++j)
                {
                    if (file)
                        file >> data.input[j];
                    else
                        {
                            file.close();
                            return false;
                        }
                }
            
            data.output.resize(outputSize);
            for (j = 0; j < outputSize; ++j)
                {
                    if (file)
                        file >> data.output[j];
                    else
                        {
                            file.close();
                            return false;
                        }
                    if (data.output[j] == 0)
                        data.output[j] = -1;
                }
            _data.push_back(data);
        }
    file.close();
    return true;
}
