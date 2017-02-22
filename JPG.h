//
// Created by vadim on 20.02.17.
//

#ifndef VJ_JPG_H
#define VJ_JPG_H
#include "Image.h"
class JPG : public Picture{
private:
    IplImage* image = 0;
    CvSize size;
public:
    //template <class Type>
    JPG(std::string &fileName, int height, int width) : Picture(fileName, height, width){
        size.height = height;
        size.width = width;
    }

    void load(){
        /*IplImage* src*/ image = cvLoadImage(fileName.c_str(), 1);
        //image = cvCreateImage(size, image->depth, src->nChannels);
        //image = src;
        //cv::resize(src, image, 0);
        w = image->width;
        h = image->height;
        //cvReleaseImage(&src);

        //w = image->width;
        //h = image->height;

        MY_RGB* palette = new MY_RGB[w*h];

        for (size_t y = 0, k = 0; y < h; y++) {
            uchar* ptr = (uchar*)(image->imageData + y * image->widthStep);
            for (size_t x = 0; x < w; x++, k++) {
                palette[k].rgbtBlue = ptr[3 * x];
                palette[k].rgbtGreen = ptr[3 * x + 1];
                palette[k].rgbtRed = ptr[3 * x + 2];
            }
        }
        toY(palette, h, w);
        std::vector<std::vector<int>> tmpMatrix = IntegralImage(matrix);
        Imatrix = tmpMatrix;
        delete[] palette;
        cvReleaseImage(&image);
    }

    std::string getExtension(){
        return "*.jpg";
    }
};
#endif //VJ_JPG_H
