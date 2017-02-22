//
// Created by vadim on 20.02.17.
//

#ifndef VJ_BOOSTING_H
#define VJ_BOOSTING_H

#include "JPG.h"
#include "PNG.h"
#include "Base.h"
#include "FFS.h"
#include "fstream"
#include <math.h>
#include "structs.h"
#include <sstream>

class Boosting{
private:
    int numbersOfPositive;//êîëëèṫåñòâî èçîáŵàæåíèé ñ ëèöàìè
    int numbersOfNegative;//êîëëèṫåñòâî èçîáŵàæåíèé áåç ëèö
    INPUT_PARAMETERS *param;

    std::ofstream fstr;
    Base* baseOfResponses;
    Base* wClassBase;
    std::vector<ARW> arw;
public:
    Boosting(INPUT_PARAMETERS *input){
        numbersOfPositive = 0;
        numbersOfNegative = 0;
        param = input;
    }

    //ṫòåíèå äèŵåêòîŵèè
    void readDirect(const char* path, bool posOrNeg){
        std::cout << "Read Directory..." << std::endl;
        Base *direct = new Base();
        direct->iterator(path);
        while (true){
            if (direct->hasNext()){
                PAIR_OF_FILENAME_AND_EXTENSION next;//èìÿ è ŵàñøèŵåíèå
                direct->next(next);//çàïîìèíàå èìÿ ôàéëà è åãî ŵàñøèŵåíèå
                PAIR_OF_FILENAME_AND_TYPE image;// ïàëèòŵà + èìÿ/ŵàñøèŵåíèå
                image.fileName = next;//çàïèñûâàåì èìÿ
                OpenFileAndSavePallete(&image);//çàïîìèíàåì ïàëèòŵó
                ARW _tmpARW;
                _tmpARW.access = image;
                _tmpARW.access.startIndex = numbersOfPositive + numbersOfNegative;//íàṫàëüíûå èíäåêñû
                _tmpARW.type = posOrNeg;
                arw.push_back(_tmpARW);
                posOrNeg ? numbersOfPositive++ : numbersOfNegative++;
            }
            else {break;}
        }
        delete direct;
        std::cout << "Finished reading!" << std::endl;
    }

    //ñëàáûé êëàññèôèêàòîŵ(íà çàïèñü)
    struct weak_classifiers{
        RESPONSE_COORDINATES_SIZE_AND_TYPE optimalResponse;
        double coefficientA;
    };

    //îáóṫåíèå
    void learning(const char* pathOfWeekClass, const char* TMPDirectory){
        wClassBase = new Base();
        wClassBase->createTmpDirectory(pathOfWeekClass);
        fstr.open("weakclassifiers.txt");
        std::cout << "Please wait..."<<std::endl;
        weightsInitialize();
        initResponses(TMPDirectory);
        for (int count = 0; count < param->numOfweakClassifiers; count++){
            std::cout << "Progress of learning: " << ((double)count / (double)param->numOfweakClassifiers) * 100 << "% / 100%"<<std::endl;
            baseOfResponses->iterator(TMPDirectory);
            size_t size_dir = baseOfResponses->getSizeDirectory();
            weightsRecalculation(arw);
            std::vector<RESPONSE_COORDINATES_SIZE_AND_TYPE> optimals;//âñå îïòèìàëüíûå ïî âñåì ïŵèçíàêàì
            for (size_t countDir = 0; countDir < size_dir; countDir++){
                PAIR_OF_FILENAME_AND_EXTENSION fileName;
                std::ostringstream name;
                name << TMPDirectory << "/" << countDir << ".tmp";
                fileName.fileName = name.str();
                fileName.extension = ".tmp";
                std::vector<RESPONSE_COORDINATES_SIZE_AND_TYPE> responses;
                baseOfResponses->readFile(fileName.fileName, responses, arw.size());
                for (size_t countRes = 0; countRes < arw.size(); countRes++){
                    arw[countRes].response = responses[arw[countRes].access.startIndex];
                }
                optimals.push_back(selectWeakClassifier(arw));
            }

            RESPONSE_COORDINATES_SIZE_AND_TYPE weekClass = getMinError(optimals); //âûáèŵàåì êëàññèôèêàòîŵ ñ ìèíèìàëüíîé îøèáêîé
            double normalizedError = weekClass.Error / (1.0 - weekClass.Error);//êîýôèöèåíò áåòòà
            for (size_t j = 0; j < arw.size(); j++){//ïåŵåñṫåò âåñîâ (ïŵîõîäèì ïî âñåé âûáîŵêå êëàññèôèêàòîŵîì è ïåŵåñṫèòûâàåì âåñà)
                std::vector<std::vector<int>> rectMatr;
                rect(arw.at(j).access.palette, weekClass.coordinate_x, weekClass.coordinate_y, weekClass.height, weekClass.width, &rectMatr);
                int resp = response(rectMatr, weekClass.numOfFeatures, weekClass.height - 1, weekClass.width - 1);
                if (arw.at(j).type == weakClassifiers(resp, weekClass.response)){
                    arw.at(j).weight *= normalizedError;
                }
            }

            double coefficient = log(1.0 / normalizedError);
            weak_classifiers wclass;
            wclass.optimalResponse = weekClass;
            wclass.coefficientA = coefficient;
            string fn =  to_string(count)+ ".vjc";
            wClassBase->createAndFillTmpFile(fn, wclass);
            fstr << wclass.optimalResponse.response << " " << wclass.optimalResponse.numOfFeatures << " " << wclass.optimalResponse.coordinate_x << " " << wclass.optimalResponse.coordinate_y << " " << wclass.optimalResponse.height << " " << wclass.optimalResponse.width << " " << wclass.coefficientA << " " << wclass.optimalResponse.Error << "\n";
            if (weekClass.Error == 0){
                std::cout << "Finished learning. Joined sufficient (" << count + 1 << ") number of classifiers. Classifiers looking directory <<classifiers>>." << endl;
                break;
            }
            if (count == param->numOfweakClassifiers - 1){
                std::cout << "Finished learning. Joined sufficient (" << param->numOfweakClassifiers << ") number of classifiers. Classifiers looking directory <<classifiers>>." << endl;
            }

        }
        delete wClassBase;
        fstr.close();
        baseOfResponses->clearTmpDirectory();
        delete baseOfResponses;
    }

protected:
    //âîçâŵàùàåò êëàññèôèêàòîŵ ñ ìèíèìàëüíûì çíàṫåíèåì îøèáêè
    RESPONSE_COORDINATES_SIZE_AND_TYPE getMinError(std::vector<RESPONSE_COORDINATES_SIZE_AND_TYPE> &optimals){
        RESPONSE_COORDINATES_SIZE_AND_TYPE min = optimals.at(0);
        for (size_t i = 0; i < optimals.size(); i++){
            if (optimals.at(i).Error < min.Error) {
                min = optimals.at(i);
            }
        }
        return min;
    }

    //çàïèñü âñåõ îòêëèêîâ ïî ôàéëàì
    void initResponses(const char* path){
        baseOfResponses = new Base();
        baseOfResponses->createTmpDirectory(path);
        int NumOfFile = 0;
        size_t size = haarsize();
        for (size_t numOfFeatures = 0; numOfFeatures < haarsize(); numOfFeatures++){
            allResponsesToTheOneFeatures(numOfFeatures, NumOfFile);
        }
    }

    //âûáîŵ ñëàáîãî êëàññèôèêàòîŵà
    RESPONSE_COORDINATES_SIZE_AND_TYPE selectWeakClassifier(std::vector<ARW> &arw){
        FFS ffs;
        ffs.errorInit(arw);
        RESPONSE_COORDINATES_SIZE_AND_TYPE optimal = ffs.getOptimalBoundary(arw);
        return optimal; //îïòèìàëüíàÿ ãŵàíèöà
    }

    //ñëàáûé êëàññèôèêàòîŵ
    bool weakClassifiers(int response, int optimal){
        return response < optimal ? true : false;
    }

    //èíèöèàëèçàöèÿ âåñîâ
    void weightsInitialize(){
        for (size_t i = 0; i < arw.size(); i++){
            arw.at(i).weight = arw.at(i).type ? (double)(1 / (double)(2 * numbersOfPositive)) : (double)(1 / (double)(2 * numbersOfNegative));
        }
    }

    //âñå îòêëèêè íà 1 ïŵèçíàê
    void allResponsesToTheOneFeatures(int numOfFeatures, int &NumOfFile){
        for (int featureHeight = param->MIN_SIZE_H; featureHeight < param->SIZE_OF_PICTURE_H - 1; featureHeight += 2){
            for (int featureWidht = param->MIN_SIZE_W; featureWidht < param->SIZE_OF_PICTURE_W - 1; featureWidht += 2){
                for (int x = 0; x < param->SIZE_OF_PICTURE_H - featureHeight; x += param->SHIFT_IN_X){
                    for (int y = 0; y < param->SIZE_OF_PICTURE_W - featureWidht; y += param->SHIFT_IN_Y){
                       std::vector<RESPONSE_COORDINATES_SIZE_AND_TYPE> responses;
                       oneFeature(responses, featureHeight, featureWidht, x, y, numOfFeatures);
                        string fn = to_string(NumOfFile) + ".tmp";
                       baseOfResponses->createAndFillTmpFile(fn, responses);
                       NumOfFile++;
                    }
                }
            }
        }
    }

    //îòêëèê êîíêŵåíîãî ïŵèçíàêà íà âñåõ èçîáŵàæåíèÿõ
    void oneFeature(std::vector<RESPONSE_COORDINATES_SIZE_AND_TYPE> &res, size_t featureHeight, size_t featureWidth, size_t x, size_t y, size_t numOfFeature){
        for (size_t i = 0; i < arw.size(); i++){
            std::vector<std::vector<int>> rectMatr;
            rect(arw.at(i).access.palette, x, y, featureHeight, featureWidth, &rectMatr);
            RESPONSE_COORDINATES_SIZE_AND_TYPE rcst;
            rcst.response = response(rectMatr, numOfFeature, featureHeight - 1, featureWidth - 1);
            rcst.coordinate_x = x;
            rcst.coordinate_y = y;
            rcst.height = featureHeight;
            rcst.width = featureWidth;
            rcst.numOfFeatures = numOfFeature;
            res.push_back(rcst);
        }
    }

    //èíèöèàëèçàöèÿ ïàëèòŵû
    void OpenFileAndSavePallete(PAIR_OF_FILENAME_AND_TYPE *pft){

        if (pft->fileName.extension == ".png"){
            PNG png(pft->fileName.fileName, param->SIZE_OF_PICTURE_H, param->SIZE_OF_PICTURE_W);
            png.load();
            pft->height = png.getHeight();
            pft->width = png.getWidth();
            pft->palette = png.getPalette();
        }
        if (pft->fileName.extension == ".jpg"){
            JPG jpg(pft->fileName.fileName, param->SIZE_OF_PICTURE_H, param->SIZE_OF_PICTURE_W);
            jpg.load();
            pft->height = jpg.getHeight();
            pft->width = jpg.getWidth();
            pft->palette = jpg.getPalette();
        }
    }

    //âûäåëåíèå ïîäìàòŵèöû
    void rect(std::vector<std::vector<int>> &matrix, size_t x, size_t y, size_t h, size_t w, std::vector<std::vector<int>> *rectMatr){
        for (size_t l = 0; l < h; l++){
            std::vector<int> temp;
            for (size_t k = 0; k < w; k++){
                temp.push_back(matrix[l + x][k + y]);
            }
            rectMatr->push_back(temp);
        }
    }

    //ïåŵåñṫåò âåñîâ
    void weightsRecalculation(std::vector<ARW> &arw){
        double sum = 0;
        for (size_t i = 0; i < arw.size(); i++){
            sum += arw.at(i).weight;
        }
        for (size_t i = 0; i < arw.size(); i++){
            arw.at(i).weight /= sum;
        }
    }

    unsigned int haarsize(){
        return HAARS;
    }

    //îòêëèê
    int response(std::vector<std::vector<int>> &rect, size_t i, size_t h, size_t w){
        int SUM_ALL;
        int SUM_BLACK;
        int SUM_WHITE;
        switch (i){
            /*ṫẁŵíàÿ ïîëîñà ñïŵàâà( ṫåŵíûé = ñóììà âñåõ - ñóììà áåëûõ)*/
            case 0:{
                SUM_WHITE = rect[h][w / 2] + rect[0][0] - rect[0][w / 2] - rect[h][0];
                SUM_BLACK = rect[h][w] + rect[0][w / 2] - rect[h][w / 2] - rect[0][w];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ âåŵòèêàëüíàÿ ïîëîñà ïî öåíòŵó(ṫåŵíûé = (áåëûé+ṫåŵíûé) - áåëûé)*/
            case 1:{
                SUM_WHITE = (rect[h][w / 3] + rect[0][0] - rect[0][w / 3] - rect[h][0]) + (rect[h][w] + rect[0][w - (w / 3)] - rect[0][w] - rect[h][w - (w / 3)]);
                SUM_BLACK = rect[h][w - (w / 3)] + rect[0][w / 3] - rect[0][w - (w / 3)] - rect[h][w / 3];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ ãîŵèçîíòàëüíàÿ ïîëîñà ïî öåíòŵó(ṫåŵíûé = (áåëûé+ṫåŵíûé) - áåëûé)*/
            case 2:{
                SUM_WHITE = (rect[h / 3][w] + rect[0][0] - rect[0][w] - rect[h / 3][0]) + (rect[h][w] + rect[h - (h / 3)][0] - rect[h - (h / 3)][w] - rect[h][0]);
                SUM_BLACK = rect[h - (h / 3)][w] + rect[h / 3][0] - rect[h / 3][w] - rect[h - (h / 3)][0];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ ïîëîñà ñíèçó(ṫåŵíûé = ñóììà âñåõ - ñàììà áåëûõ)*/
            case 3:{
                SUM_WHITE = rect[h / 2][w] + rect[0][0] - rect[0][w] - rect[h / 2][0];
                SUM_BLACK = rect[h][w] + rect[h / 2][0] - rect[h / 2][w] - rect[h][0];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                //êâàäŵàòèê ïî öåíòŵó
            case 4:{
                SUM_BLACK = rect[h - (h / 3)][w - (w / 3)] + rect[h / 3][w / 3] - rect[h / 3][w - (w / 3)] - rect[h - (h / 3)][w / 3];
                SUM_WHITE = rect[h][w] + rect[0][0] - rect[h][0] - rect[0][w] - SUM_BLACK;
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                //øàøåṫêè
            case 5:{
                SUM_WHITE = (rect[h / 2][w / 2] + rect[0][0] - rect[0][w / 2] - rect[h / 2][0]) + (rect[h][w] + rect[h / 2][w / 2] - rect[h / 2][w] - rect[h][w / 2]);
                SUM_BLACK = rect[h][w] + rect[0][0] - rect[h][0] - rect[0][w] - SUM_WHITE;
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
            default:{
                std::cout << "Error: incorrect response" << std::endl;
                return NULL;
            }
        }
    }
};
#endif //VJ_BOOSTING_H
