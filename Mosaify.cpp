#include "Mosaify/Mosaify.h"
#include "Image.h"
#include "Color.h"

#include <iostream>
#include <thread>
#include <mutex>

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

#if defined(__linux__)
#include <fstream>
#include <sstream>

size_t getMemoryUsage() {
    std::ifstream proc("/proc/self/status");
    std::string line;
    while (std::getline(proc, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            // Found the line that contains memory usage info
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            return std::stol(value) * 1024; // Convert kB to bytes
        }
    }
    return 0; // If VmRSS line is not found
}

#elif defined(__APPLE__)
#include <mach/mach.h>

size_t getMemoryUsage() {
    struct task_basic_info info;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS) {
        return 0;
    }

    return info.resident_size;
}

#else
#error "Unsupported platform"
#endif

std::string formatWithSIUnit(double bytes) {
    const char* suffixes[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};
    int suffixIndex = 0;
    double value = bytes;

    while (value >= 1024 && suffixIndex < 6) {
        value /= 1024;
        ++suffixIndex;
    }

    char formattedValue[50];
    sprintf(formattedValue, "%.2f %s", value, suffixes[suffixIndex]);
    return std::string(formattedValue);
}

const NJLIC::Image *ImageFileLoader::load(const string& filename) {

    int width, height, channels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data) {
        throw runtime_error("Failed to load image: " + filename);
    }

    NJLIC::Image *image = new NJLIC::Image ;
    image->copyData(data, width, height, channels, filename);

    stbi_image_free(data);

    return image;
}

void ImageFileLoader::write(const string &filename, const NJLIC::Image *img) {
    stbi_write_png(filename.c_str(),
                   img->getWidth(),
                   img->getHeight(),
                   img->getNumberOfComponents(),
                   img->getDataPtr(),
                   img->getNumberOfComponents() * img->getWidth());
}

// Define a global mutex to protect the image map
static mutex creationMutex;

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

static std::string extractPath(const std::string &filepath) {
    size_t pos = filepath.find_last_of("/\\");

    if (pos != std::string::npos) {
        // return substring after last '/' or '\' character
        return filepath.substr(0, pos);
    } else {
        // no '/' or '\' character found, return the whole string
        return filepath;
    }

}

// Define a function to calculate the similarity between two images
static double calculateSimilarity(const NJLIC::Image *target, int image1OffsetX, int image1OffsetY, int image1SizeX, int image1SizeY,
                                  const NJLIC::Image *image) {
    int sum = 0;
    for (int y = 0; y < image->getHeight(); y++) {
        for (int x = 0; x < image->getWidth(); x++) {

            glm::vec4 pixel1;
            image->getPixel(glm::vec2(x, y), pixel1);
            NJLIC::Color color1;
            color1.setRGB(pixel1);

            glm::vec4 pixel2;
            target->getPixel(glm::vec2(x + image1OffsetX, y + image1OffsetY), pixel2);
            NJLIC::Color color2;
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
static const NJLIC::Image &generateMosaic(const NJLIC::Image *targetImage, vector<Mosaify::TileImage> &images, int tileSize, int numThreads, Mosaify::MosaicMap &mmap) {
    int targetWidth = targetImage->getWidth();
    int targetHeight = targetImage->getHeight();
    int tileCols = targetWidth / tileSize;
    int tileRows = targetHeight / tileSize;

    static NJLIC::Image mosaicPixels(*targetImage);

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

                double similarity = calculateSimilarity(targetImage, tileX, tileY, tileSize, tileSize, images[i].second);
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

            creationMutex.lock();
            mosaicPixels.setPixels(glm::vec2(tileX, tileY), images[bestMatchIndex].second);
            mmap.insert(Mosaify::MosaicMapPair(Mosaify::Indices(tileX / tileSize, tileY / tileSize), images[bestMatchIndex].first));
            creationMutex.unlock();
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

const NJLIC::Image *Mosaify::resizeImage(const NJLIC::Image *img)const {
    NJLIC::Image *ret = new NJLIC::Image(*img);
    ret->resize(mTileSize, mTileSize);
    return ret;
}

Mosaify::Mosaify() :
mTileSize(8),
mMaxHeight(0),
mMaxWidth(0),
mMosaicImage(new NJLIC::Image())
{
    mMosaicImage->generate(mTileSize, mTileSize, 3);
}

Mosaify::~Mosaify() {
    delete mMosaicImage;
    while (!mTileImages.empty()) {
        NJLIC::Image *img = mTileImages.back().second;
        delete img;
        mTileImages.pop_back();
    }
}

void Mosaify::setTileSize(int tileSize) {
    mTileSize = tileSize;
}

int Mosaify::getTileSize()const {
    return mTileSize;
}

void Mosaify::addTileImage(int width,
                           int height,
                           int components,
                           uint8 *data,
                           const char *filepath,
                           TileId id) {
    NJLIC::Image *img = new NJLIC::Image();
    img->copyData(data, width, height, components, filepath);
    if(width > mMaxWidth)mMaxWidth=width;
    if(height > mMaxHeight)mMaxHeight=height;
    mTileImages.push_back(TileImage(id, img));
}

bool Mosaify::removeTileImage(TileId id) {
    bool ret = false;
    for(auto iter = mTileImages.begin(); iter != mTileImages.end();) {
        if((*iter).first == id) {
            iter = mTileImages.erase(iter);
            ret = true;
        } else {
            iter++;
        }
    }
    return ret;
}

bool Mosaify::hasTileImage(TileId id)const {
    for(auto iter = mTileImages.begin(); iter != mTileImages.end();) {
        if((*iter).first == id) {
            return true;
        } else {
            iter++;
        }
    }
    return false;
}

bool Mosaify::getTileImage(TileId id, NJLIC::Image &img)const {
    for(auto iter = mTileImages.begin(); iter != mTileImages.end();) {
        if((*iter).first == id) {
            img = *((*iter).second);
            return true;
        } else {
            iter++;
        }
    }
    return false;
}

bool Mosaify::updateTileImage(int width,
                              int height,
                              int components,
                              uint8 *data,
                              const char *filepath,
                              TileId id) {
    bool ret = false;
    for(auto iter = mTileImages.begin(); iter != mTileImages.end();) {
        if((*iter).first == id) {
            auto oldImg = (*iter).second;

            NJLIC::Image *img = new NJLIC::Image();
            img->copyData(data, width, height, components, filepath);
            (*iter).second = img;

            delete oldImg;

            ret = true;
        } else {
            iter++;
        }
    }
    return ret;

}

bool Mosaify::generate(int width,
                       int height,
                       int components,
                       uint8 *data) {
    int numThreads = getMaxThreads();
    NJLIC::Image *targetImage = nullptr;

    std::cout << "Memory usage: " << formatWithSIUnit(getMemoryUsage()) << std::endl;


    try
    {
        targetImage = new NJLIC::Image();
        targetImage->copyData(data, width, height, components, "loaded");
    }
    catch (std::bad_alloc & ba)
    {
        throw std::runtime_error(std::string("bad_alloc caught: ") + std::string(ba.what()));
    }
    catch (...)
    {
        throw std::runtime_error(std::string("exception caught"));
    }

    vector<Mosaify::TileImage> images;
    // Have to resized the tiles so that it can be tiled correctly.
    int i = 0;
    size_t totalSize = getMemoryUsage();
    std::cout << "Memory usage: " << formatWithSIUnit(totalSize) << std::endl;
    for(auto iter = mTileImages.begin(); iter != mTileImages.end(); iter++) {

        try
        {
            NJLIC::Image *img = new NJLIC::Image(*((*iter).second));
            TileId id = (*iter).first;

            img->resize(mTileSize, mTileSize);
            images.push_back(Mosaify::TileImage(id, img));
        }
        catch (std::bad_alloc & ba)
        {
            throw std::runtime_error(std::string("bad_alloc caught: ") + std::string(ba.what()));
        }
        catch (...)
        {
            throw std::runtime_error(std::string("exception caught"));
        }
        std::cout << "Image " << i << " of " << mTileImages.size() << std::endl;
        size_t currentSize = getMemoryUsage();
        std::cout << "\tMemory allocated: " << formatWithSIUnit(currentSize - totalSize) << std::endl;
        totalSize = getMemoryUsage();
        std::cout << "\tMemory usage: " << formatWithSIUnit(totalSize) << std::endl;
    }
    mMosaicMap.clear();

    *mMosaicImage = generateMosaic(targetImage, images, mTileSize, numThreads, mMosaicMap);
    std::cout << "Memory usage: " << formatWithSIUnit(getMemoryUsage()) << std::endl;

    while(!images.empty()) {
        Mosaify::TileImage ti = images.back();
        images.pop_back();
        delete ti.second;
    }


    delete targetImage;
    std::cout << "Memory usage: " << formatWithSIUnit(getMemoryUsage()) << std::endl;

    return true;
}

void Mosaify::getMosaicImage(NJLIC::Image &img)const {
   img = *mMosaicImage;
}

const char *Mosaify::getMosaicMap()const {
    static string ret("{}");
    json j;

    // iterate through the MosaicMap and add each pair to the JSON object
    for (const auto& pair : mMosaicMap) {
        Indices indices = pair.first;
        TileId tid = pair.second;

        // create a JSON object for the pair and add it to the JSON array
        j.push_back({
                            {"x", indices.first},
                            {"y", indices.second},
                            {"id", tid}
                    });
    }

    if(j.empty()) return ret.c_str();

    ret = j.dump();
    return ret.c_str();
}

const char *Mosaify::getMosaicJsonArray()const {
    int targetWidth = mMosaicImage->getWidth();
    int targetHeight = mMosaicImage->getHeight();
    int cols = targetWidth / getTileSize();
    int rows = targetHeight / getTileSize();

    std::vector<std::vector<std::string>> grid;
    grid.resize(rows);
    for(int i = 0; i < rows; i++) {
        grid[i].resize(cols);
    }

    for (const auto& pair : mMosaicMap) {
        Indices indices = pair.first;
        TileId tid = pair.second;

        grid[indices.second][indices.first] = tid;
    }

    int i = 0;
    static string val = "[";
    for(i = 0; i < grid.size()-1; i++) {

        string fileName;
        int j = 0;
        string string_row = "[";
        for(j = 0; j < grid[i].size()-1; j++) {
            fileName = grid[i][j];
            string_row += string("\"") + fileName + string("\", ");
        }
        string_row += string("\"") + fileName + string("\"");
        string_row += "],\n";
        val += string_row;
    }

    string fileName;
    int j = 0;
    string string_row = "[";
    for(j = 0; j < grid[i].size()-1; j++) {
        fileName = grid[i][j];
        string_row += string("\"") + fileName + string("\", ");
    }
    string_row += string("\"") + fileName + string("\"");
    string_row += "]\n";
    val += string_row;
    val += string("]");

    return val.c_str();
}

void Mosaify::getMosaicMap(Mosaify::MosaicMap &map)const {
    map = mMosaicMap;
}
