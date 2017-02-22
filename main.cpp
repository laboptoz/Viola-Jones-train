#include "Boosting.h"
int main(int argc, char** argv){
    setlocale(LC_ALL, "rus");
    if ( argc != 12 )
    {
        printf("usage: ./VJ <nomOfWeakClassifiers> <H> <W> <minH> <minW> <ShiftX> <ShiftY> <PathPosIm> <PathNegIm> <PathWeakClass> <tmpDirectory>\n");
        return -1;
    }
    VJ_BOOSTING_H::INPUT_PARAMETERS *input = new INPUT_PARAMETERS();
    input->numOfweakClassifiers = atoi(argv[1]);
    input->SIZE_OF_PICTURE_H = atoi(argv[2]);
    input->SIZE_OF_PICTURE_W = atoi(argv[3]);
    input->MIN_SIZE_H = atoi(argv[4]);
    input->MIN_SIZE_W = atoi(argv[5]);
    input->SHIFT_IN_X = atoi(argv[6]);
    input->SHIFT_IN_Y = atoi(argv[7]);
    const char *pos = argv[8];
    const char *neg = argv[9];
    const char *wclass = argv[10];
    const char *tmpDirectory = argv[11];
    Boosting adaboost(input);
    adaboost.readDirect(pos, true);
    adaboost.readDirect(neg, false);
    adaboost.learning(wclass, tmpDirectory);
    delete input;
    return 0;
}