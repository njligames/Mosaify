#include "Mosaify/Mosaify.h"
#include "Image.h"
#include "Color.h"

#include <iostream>
#include <thread>

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
using Image = NJLIC::Image;

// Define a global mutex to protect the image map
static mutex imageMapMutex;

static std::string extractFilename(const std::string& filepath) {
    // find last '/' or '\' character in filepath
    size_t pos = filepath.find_last_of("/\\");

    if (pos != std::string::npos) {
        // return substring after last '/' or '\' character
        return filepath.substr(pos + 1);
    } else {
        // no '/' or '\' character found, return the whole string
        return filepath;
    }
}

// Define a function to calculate the similarity between two images
static double calculateSimilarity(const Image *target, int image1OffsetX, int image1OffsetY, int image1SizeX, int image1SizeY,
                                  const Image *image) {
    int sum = 0;
    for (int y = 0; y < image->getHeight(); y++) {
        for (int x = 0; x < image->getWidth(); x++) {

            glm::vec4 pixel1;
            image->getPixel(glm::vec2(x, y), pixel1);
            Color color1;
            color1.setRGB(pixel1);

            glm::vec4 pixel2;
            target->getPixel(glm::vec2(x + image1OffsetX, y + image1OffsetY), pixel2);
            Color color2;
            color2.setRGB(pixel2);

            int diff = color1.distance(color2);

            sum += diff * diff;
        }
    }

    double mse = (double) sum / (image->getWidth() * image->getHeight() * image->getNumberOfComponents());
    double rmse = sqrt(mse);
    return (int) rmse;
}

// Define a function to generate a mosaic image
static Image &generateMosaic(const Image *targetImage, const vector<Image*>& images, int tileSize, int numThreads, Mosaify::MosaicMap &mmap) {
    int targetWidth = targetImage->getWidth();
    int targetHeight = targetImage->getHeight();
    int tileCols = targetWidth / tileSize;
    int tileRows = targetHeight / tileSize;

    Image mosaicPixels(*targetImage);
//    mosaicPixels.copyData(targetImagegetDataPtr(), targetImage.getWidth(), targetImage.getHeight(), targetImage.getNumberOfComponents(), targetImage.getFilename());

    // Create a vector to hold the similarity scores
    vector<vector<double>> similarityScores(images.size(), vector<double>(tileCols * tileRows));

    // Define a function to calculate the similarity scores in parallel
    auto calculateSimilarityScores = [&](int threadIndex) {
        for (int i = threadIndex; i < images.size(); i += numThreads) {
            for (int j = 0; j < tileCols * tileRows; j++) {

                int tileRow = j / tileCols;
                int tileCol = j % tileCols;
                int tileX = tileCol * tileSize;
                int tileY = tileRow * tileSize;

                double similarity = calculateSimilarity(targetImage, tileX, tileY, tileSize, tileSize, images[i]);
                similarityScores[i][j] = similarity;
            }
        }
    };

    // Calculate the similarity scores in parallel
    vector<thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(thread(calculateSimilarityScores, i));
    }
    for (auto& t : threads) {
        t.join();
    }

    // Define a function to find the best match for each tile in parallel
    auto findBestMatch = [&](int threadIndex) {
        for (int j = threadIndex; j < tileCols * tileRows; j += numThreads) {

            // Get the tile coordinates
            int tileRow = j / tileCols;
            int tileCol = j % tileCols;
            int tileX = tileCol * tileSize;
            int tileY = tileRow * tileSize;

            // Find the best matching image for the tile
            int bestMatchIndex = 0;
            int bestMatchScore = similarityScores[0][j];
            for (int i = 1; i < images.size(); i++) {
                int similarity = similarityScores[i][j];
                if (similarity < bestMatchScore) {
                    bestMatchIndex = i;
                    bestMatchScore = similarity;
                }
            }

            mosaicPixels.setPixels(glm::vec2(tileX, tileY), images[bestMatchIndex]);

            imageMapMutex.lock();
            mmap.insert(Mosaify::MosaicMapPair(Mosaify::Indices(tileX / tileSize, tileY / tileSize), extractFilename(images[bestMatchIndex]->getFilename())));
            imageMapMutex.unlock();
        }
    };

    // Find the best match for each tile in parallel
    threads.clear();
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(thread(findBestMatch, i));
    }
    for (auto& t : threads) {
        t.join();
    }

    return mosaicPixels;
}

int Mosaify::getMaxThreads()const {
    int maxThreads = thread::hardware_concurrency();
    if (maxThreads == 0) {
        cerr << "Unable to determine the number of hardware threads. Using default value of 1." << endl;
        maxThreads = 1;
    }
    return maxThreads;
}

Image *Mosaify::resizeImage(const Image *img)const {
    Image *ret = new Image(*img);
    ret->resize(mTileSize, mTileSize);
    return ret;
}

Mosaify::Mosaify() {
    cout << "Mosaify instance created." << endl;
}

Mosaify::~Mosaify() {
    while (!mTileImages.empty()) {
        Image *img = mTileImages.back();
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

    Image *img = new Image();
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
        Image *img = mTileImages.at(idx);

        uint32 size = ((img->getWidth()) * (img->getHeight()) * (img->getNumberOfComponents()));
        si->imgdata = new unsigned char[size];
        memcpy(si->imgdata, img->getDataPtr(), size);

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
    int numThreads = getMaxThreads();

    Image *targetImage = new Image();
    targetImage->copyData(data, width, height, components, "loaded");

    // Have to resized the tiles so that it can be tiled correctly.
    vector<Image*> resizedImages;
    for(auto iter = mTileImages.begin(); iter != mTileImages.end(); iter++) {
        resizedImages.push_back(resizeImage(*iter));
    }
    mMosaicMap.clear();

    // TODO: generate
    Image generatedMosaicImage = generateMosaic(targetImage, resizedImages, mTileSize, numThreads, mMosaicMap);

    while(!resizedImages.empty()) {
       Image *img = resizedImages.back();
       delete img;
       resizedImages.pop_back();
    }
    delete targetImage;


//    mosaicMap_.clear();

//
//    mosaicImage_.data = (unsigned char*)sImage.getDataPtr();
//    mosaicImage_.width = sImage.getWidth();
//    mosaicImage_.height = sImage.getHeight();
//    mosaicImage_.components = sImage.getNumberOfComponents();
//    mosaicImage_.id = sImage.getFilename();

    return true;
}

bool Mosaify::generate(const simple_image *si) {
   return this->generate(si->rows, si->cols, si->comp, si->imgdata);
}

const char * Mosaify::getMosaicMap()const {
    static string ret;
    json j;

    // iterate through the MosaicMap and add each pair to the JSON object
    for (const auto& pair : mMosaicMap) {
        Indices indices = pair.first;
        string filename = pair.second;

        // create a JSON object for the pair and add it to the JSON array
        j.push_back({
                            {"x", indices.first},
                            {"y", indices.second},
                            {"filename", filename}
                    });
    }

    if(j.empty()) return "{}";
    ret = j.dump();
    return ret.c_str();
}
