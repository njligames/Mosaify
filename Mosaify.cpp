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
