//
// Created by vadim on 20.02.17.
//

#ifndef VJ_STRUCTS_H
#define VJ_STRUCTS_H
#include <vector>
#include <iostream>
#define HAARS 6
typedef unsigned char byte;
//âõîäíûå ïàŵàìåòŵû
void print(std::vector<std::vector<int>> rec){
    for (int i = 0; i < rec.size(); i++){
        for (int j = 0; j < rec.at(i).size(); j++){
            std::cout << rec.at(i).at(j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print(std::vector<std::vector<byte>> rec){
    for (int i = 0; i < rec.size(); i++){
        for (int j = 0; j < rec.at(i).size(); j++){
            std::cout << (unsigned int)rec.at(i).at(j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
struct INPUT_PARAMETERS{
    int numOfweakClassifiers;
    int SIZE_OF_PICTURE_H;
    int SIZE_OF_PICTURE_W;
    int MIN_SIZE_H;
    int MIN_SIZE_W;
    int SHIFT_IN_X;
    int SHIFT_IN_Y;
};

struct RESPONSE_COORDINATES_SIZE_AND_TYPE{
    int response;
    size_t coordinate_x;
    size_t coordinate_y;
    size_t height;
    size_t width;
    size_t numOfFeatures;
    double Error;
};

//èìÿ ôàéëà è ŵàñøèŵåíèå
struct PAIR_OF_FILENAME_AND_EXTENSION{
    std::string fileName;//èìÿ ôàéëà
    std::string extension;//ŵàñøèŵåíèå
};

//ïàëèòŵà, èìÿ/ŵàñøèŵåíèå, ëèöî/íåëèöî
struct PAIR_OF_FILENAME_AND_TYPE{
    std::vector<std::vector<int>>  palette;//ïàëèòŵà
    size_t height; //ŵàçìåŵû ïàëèòŵû
    size_t width;
    PAIR_OF_FILENAME_AND_EXTENSION fileName;//èìÿ + ŵàñøèŵåíèå
    size_t startIndex;
};

struct MY_RGB{
    byte rgbtBlue;
    byte rgbtGreen;
    byte rgbtRed;
};

struct Point{ // òîṫêà
    double h;
    double w;
};
struct Sign{
    std::vector<Point> cordinates;// òîṫêè
};

struct ARW{
    PAIR_OF_FILENAME_AND_TYPE access;//áàçà
    RESPONSE_COORDINATES_SIZE_AND_TYPE response;//îòêëèê
    double weight;//âåñ
    bool type;//ñ ëèöîì/áåç ëèöà
};
#endif //VJ_STRUCTS_H
