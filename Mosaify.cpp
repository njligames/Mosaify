#include "Mosaify/Mosaify.h"
#include "Image.h"

#include <iostream>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "nlohmann/json.hpp"
using json = nlohmann::json;

Mosaify::Mosaify() {
    cout << "Mosaify instance created." << endl;
}

Mosaify::~Mosaify() {
    while (!mTileImages.empty()) {
        NJLIC::Image *img = mTileImages.back();
        delete img;
        mTileImages.pop_back();
    }
}

void Mosaify::setTileSize(int tileSize) {
    mTileSize = tileSize;
}

uint32 Mosaify::addTileImage(int width,
                           int height,
                           int components,
                           uint8 *data) {
    uint32 idx = mTileImages.size();

    NJLIC::Image *img = new NJLIC::Image();
    img->copyData(data, width, height, components, "loaded");
    mTileImages.push_back(img);

    return idx;
}

uint32 Mosaify::addTileImage(const simple_image* si) {
    return this->addTileImage(si->rows, si->cols, si->comp, si->imgdata);
}

simple_image *Mosaify::getTileImage(uint32 idx)const {
    simple_image *si = new simple_image();
    if(idx < mTileImages.size()) {
        NJLIC::Image *img = mTileImages.at(idx);

        uint32 size = ((img->getWidth()) * (img->getHeight()) * (img->getNumberOfComponents()));
        si->imgdata = new unsigned char[size];
        memcpy(si->imgdata, img->getDataPtr(), size);


//        si->imgdata = (uint8*)img->getDataPtr();
        si->comp = img->getNumberOfComponents();
        si->rows = img->getHeight();
        si->cols = img->getWidth();
    }
    return si;
}

bool Mosaify::generate(int width,
              int height,
              int components,
              unsigned char *data) {
    return false;
}

bool Mosaify::generate(const simple_image *si) {
   return this->generate(si->rows, si->cols, si->comp, si->imgdata);
}
const char * Mosaify::getMosaicMap()const {
    json j;

    // iterate through the MosaicMap and add each pair to the JSON object
    for (const auto& pair : mosaicMap) {
        Indices indices = pair.first;
        string filename = pair.second;

        // create a JSON object for the pair and add it to the JSON array
        j.push_back({
                            {"x", indices.first},
                            {"y", indices.second},
                            {"filename", filename}
                    });
    }

    return j.dump();

    return "NO";
}
