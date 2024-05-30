#ifndef MOSAICIMAGECREATOR_LIBRARY_H
#define MOSAICIMAGECREATOR_LIBRARY_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

using namespace std;

typedef unsigned int uint32;
typedef unsigned char uint8;

typedef struct simple_image_t {
    uint32 rows;
    uint32 cols;
    uint32 comp;
    uint8 *imgdata;
} simple_image;

namespace NJLIC {
    class Image;
}

class Mosaify {
public:
    typedef pair<int, int> Indices;
    typedef map<Indices, string> MosaicMap;
    typedef pair<Indices, string> MosaicMapPair;
private:
    vector<NJLIC::Image*> mTileImages;
    uint8 mTileSize;

    MosaicMap mMosaicMap;

    int getMaxThreads()const;
    NJLIC::Image *resizeImage(const NJLIC::Image *img)const;
public:
    Mosaify();
    ~Mosaify();

    void setTileSize(int tileSize);

    uint32 addTileImage(int width,
                      int height,
                      int components,
                      uint8 *data);
    uint32 addTileImage(const simple_image* si);

    simple_image *getTileImage(uint32 idx)const;

    bool generate(int width,
                  int height,
                  int components,
                  unsigned char *data);
    bool generate(const simple_image *si);

    const char * getMosaicMap()const;
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
