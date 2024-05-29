#include "Mosaify/Mosaify.h"

#include <iostream>

Mosaify::Mosaify() {
    std::cout << "Mosaify instance created." << std::endl;
}

void Mosaify::setTileSize(int tileSize) {

}

void Mosaify::addTileImage(int width,
                  int height,
                  int components,
                  unsigned char *data) {

}

//void some_func(const simple_image* si)
//{
//    uint32 i,j;
//
//    printf(
//            "rows = %d\n"
//            "cols = %d\n",
//            si->rows,si->cols);
//
//    /* Dump a simple map of the image data */
//    for(i = 0; i < si->rows; i++)
//    {
//        for(j = 0; j < si->cols; j++)
//        {
//            if(si->imgdata[i * si->rows + j] < 0x80)
//                printf(" ");
//            else
//                printf("*");
//        }
//        printf("\n");
//    }
//}

void Mosaify::addTileImage(const simple_image* si) {
    printf( "rows = %d cols = %d\n", si->rows,si->cols);

    /* Dump a simple map of the image data */
}

void Mosaify::addTileImage(const char *file) {

}

bool Mosaify::generate(int width,
              int height,
              int components,
              unsigned char *data) {
    return false;
}

bool Mosaify::generate(const char *file) {
    return false;
}
