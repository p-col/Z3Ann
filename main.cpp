
#include <iostream>
#include <vector>
#include "z3++.h"
#include "z3ann.hpp"
#include "Z3SolutionConverter.hpp"
#include "BackendC.hpp"

int main()
{
    std::cout << "+-------------------------------+" << std::endl;
    std::cout << "|          XOR example          |" << std::endl;
    std::cout << "+-------------------------------+" << std::endl;
    std::cout << "|     Set 1: Input{-1, -1}      |" << std::endl;
    std::cout << "|     Set 2: Input{ 1, -1}      |" << std::endl;
    std::cout << "|     Set 3: Input{-1,  1}      |" << std::endl;
    std::cout << "|     Set 4: Input{ 1,  1}      |" << std::endl;
    std::cout << "+-------------------------------+" << std::endl;
    std::cout << "| 0 -> -1 | activation :        |" << std::endl;
    std::cout << "| 1 ->  1 | -> Sum(Ii* Wij) > 0 |" << std::endl;
    std::cout << "|         |      then 1 else -1 |" << std::endl;
    std::cout << "+---------+---------------------+" << std::endl;
    std::cout << "|               o2              |" << std::endl;
    std::cout << "|              / \\              |" << std::endl;
    std::cout << "| Input 1 ->  o0  \\             |" << std::endl;
    std::cout << "|              \\   \\            |" << std::endl;
    std::cout << "|              3o---o5 -> Output|" << std::endl;
    std::cout << "|              /   /            |" << std::endl;
    std::cout << "| Input 2 ->  o1  /             |" << std::endl;
    std::cout << "|              \\ /              |" << std::endl;
    std::cout << "|               o4              |" << std::endl;
    std::cout << "+-------------------------------+" << std::endl << std::endl;

    std::vector<trainingData> data;
    data.resize(4);
    data[0].input.resize(2);
    data[1].input.resize(2);
    data[2].input.resize(2);
    data[3].input.resize(2);

    data[0].output.resize(1);
    data[1].output.resize(1);
    data[2].output.resize(1);
    data[3].output.resize(1);

    data[0].input[0] = -1;
    data[0].input[1] = -1;
    data[0].output[0] = -1;
    
    data[1].input[0] = 1;
    data[1].input[1] = -1;
    data[1].output[0] = 1;

    data[2].input[0] = -1;
    data[2].input[1] = 1;
    data[2].output[0] = 1;

    data[3].input[0] = 1;
    data[3].input[1] = 1;
    data[3].output[0] = -1;

    z3ann ann(2, 3, 1, -1, 1);
    ann.setAllLink(false);

    // I1
    ann.setLink(0, 2, true);
    ann.setLink(0, 3, true);
    // ann.setLink(0, 4, true);

    // I2
    // ann.setLink(1, 2, true);
    ann.setLink(1, 3, true);
    ann.setLink(1, 4, true);

    // hidden to output layer
    ann.setLink(2, 5, true);
    ann.setLink(3, 5, true);
    ann.setLink(4, 5, true);

    // ann.setAllLink(true);
    ann.loadTrainingDataSet(data);
    ann.solve();


    std::cout << "========= C Backend Output ==========" << std::endl;
    BackendC output(ann);
    std::cout << output.getOutput();
    return 0;
}
