//
// Created by vadim on 20.02.17.
//

#ifndef VJ_IMAGE_H
#define VJ_IMAGE_H
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "structs.h"

typedef  unsigned char byte;

const byte WHITE = 255;
const byte BLACK = 0;

class Picture{

protected:
    std::string fileName;
    size_t h, w;
    std::vector<std::vector<byte>>  matrix;
    std::vector<std::vector<int>>  Imatrix;

    int getIntegralPixel(std::vector<std::vector<byte>> &im, std::vector<std::vector<int>> &inIm, size_t x, size_t y){
        int intPix = 0;
        int C = (int)im.at(x).at(y);
        if (x == 0){
            if (y == 0){
                intPix = C;
            }
            else{
                int B = inIm.at(x).at(y - 1);
                intPix = C + B;
            }
        }
        else if (y == 0){
            int D = inIm.at(x - 1).at(y);
            intPix = C + D;
        }
        else{
            int A = inIm.at(x - 1).at(y - 1);
            int B = inIm.at(x).at(y - 1);
            int D = inIm.at(x - 1).at(y);
            intPix = C - A + B + D;
        }
        return intPix;
    }

    std::vector<std::vector<int>> IntegralImage(std::vector<std::vector<byte>> &im){
        std::vector<std::vector<int>> inIm;
        for (size_t i = 0; i < im.size(); i++){
            std::vector<int> w(im.at(0).size());
            inIm.push_back(w);
        }
        for (size_t i = 0; i < inIm.size(); i++){
            for (size_t j = 0; j < inIm.at(i).size(); j++){
                inIm.at(i).at(j) = getIntegralPixel(im, inIm, i, j);
            }
        }
        return inIm;
    }

    void toY(MY_RGB *palette, size_t h, size_t w){
        size_t hw = 0;
        for (size_t i = 0; i < h; i++){
            std::vector<byte> temp;
            for (size_t j = 0; j < w; j++){
                temp.push_back((byte)(0.299*(double)palette[hw].rgbtRed) + (0.587*(double)palette[hw].rgbtGreen) + (0.114*(double)palette[hw].rgbtBlue + 0.5));
                hw++;
            }
            matrix.push_back(temp);
        }
        //print(matrix);
    }

public:
    //template <class Type>
    Picture(std::string &name, int, int) : fileName(name) {}

    virtual void load() = 0;

    virtual std::string getExtension() = 0;

    size_t  getHeight(){ return this->h; }

    size_t getWidth() { return this->w; }

    std::vector<std::vector<int>>  getPalette(){ return this->Imatrix; }

    std::string getName(){ return this->fileName; }

};

#endif //VJ_IMAGE_H
