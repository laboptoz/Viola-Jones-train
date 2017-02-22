//
// Created by vadim on 20.02.17.
//

#ifndef VJ_FFS_H
#define VJ_FFS_H
#include <fstream>
#include "structs.h"
#include <algorithm>
class FFS {
private:
    double error;

public:
    FFS() {
        error = 0;
    }

    //íàṫàëüíîå çíàṫåíèå îøèáêè
    void errorInit(std::vector<ARW> &arw) {
        for (size_t i = 0; i < arw.size(); i++) {
            if (arw.at(i).type) {
                error += arw.at(i).weight;
            }
        }
    }

    struct comp {
        bool operator()(ARW i, ARW j) { return (i.response.response < j.response.response); }
    } func;

    //âîçâŵàùàåò îïòèìàëüíûé ïŵèçíàê
    RESPONSE_COORDINATES_SIZE_AND_TYPE getOptimalBoundary(std::vector<ARW> &arw) {
        std::sort(arw.begin(), arw.end(), func);
        std::vector<double> currentsErrors;
        for (size_t i = 0; i < arw.size(); i++) {
            if (arw.at(i).type) {
                error -= arw.at(i).weight;
                if (error < 0) {
                    std::cout << "!";;
                    //error = 0;
                }

            }
            if (!arw.at(i).type) {
                error += arw.at(i).weight;
                if (error > 1) {
                    std::cout << "?";
                    //error = 1;

                }
            }
            currentsErrors.push_back(error);
        }
        size_t numOfMinimumError = getNumOfMinValue(currentsErrors);
        arw.at(numOfMinimumError).response.Error = currentsErrors.at(numOfMinimumError);
        return arw.at(numOfMinimumError).response;
    }

protected:

    //âîçâŵàùàåò çíàṫåíèå ñ ìèíèìàëüíîé îøèáêîé
    size_t getNumOfMinValue(std::vector<double> &mass) {
        double minimum = mass.at(0);
        size_t numOfMinimum = 0;
        for (size_t i = 0; i < mass.size(); i++) {
            if (mass.at(i) < minimum) {
                minimum = mass.at(i);
                numOfMinimum = i;
            }
        }
        return numOfMinimum;
    }
};
#endif //VJ_FFS_H
