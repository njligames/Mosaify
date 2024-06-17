#ifndef MOSAICIMAGECREATOR_LIBRARY_H
#define MOSAICIMAGECREATOR_LIBRARY_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

using namespace std;

typedef unsigned char uint8;

namespace NJLIC {
    class Image;
}

class ImageFileLoader {
public:
    static const NJLIC::Image *load(const string &filename);
    static void write(const string &filename, const NJLIC::Image *img);
};

class Mosaify {
public:
    typedef long long int TileId;

    typedef pair<unsigned int, unsigned int> Indices;
    typedef map<Indices, TileId> MosaicMap;
    typedef pair<Indices, TileId> MosaicMapPair;

    typedef pair<TileId, NJLIC::Image*> TileImage;
private:
    vector<TileImage> mTileImages;
    uint8 mTileSize;
    int mMaxHeight;
    int mMaxWidth;

    MosaicMap mMosaicMap;
    NJLIC::Image *mMosaicImage;

    int getMaxThreads()const;
    const NJLIC::Image *resizeImage(const NJLIC::Image *img)const;
public:
    Mosaify();
    ~Mosaify();

    int getMaxWidth()const{return mMaxWidth;}
    int getMaxHeight()const{return mMaxHeight;}



    void setTileSize(int tileSize);
    int getTileSize()const;

    void addTileImage(int width,
                        int height,
                        int components,
                        uint8 *data,
                        const char *filepath,
                        TileId id);
    bool removeTileImage(TileId id);

    bool hasTileImage(TileId id)const;

    bool getTileImage(TileId id, NJLIC::Image &img)const;

    bool updateTileImage(int width,
                         int height,
                         int components,
                         uint8 *data,
                         const char *filepath,
                         TileId id);

    bool generate(int width,
                  int height,
                  int components,
                  uint8 *data);

    void getMosaicImage(NJLIC::Image &img)const;

    const char *getMosaicMap()const;
    const char *getMosaicJsonArray()const;

    void getMosaicMap(MosaicMap &vec)const;
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
