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
    unsigned char mTileSize;

    MosaicMap mMosaicMap;
    NJLIC::Image *mMosaicImage;
    string mMosaicPreview;

    int getMaxThreads()const;
    const NJLIC::Image *resizeImage(const NJLIC::Image *img)const;
public:
    Mosaify();
    ~Mosaify();

    const char * getMosaicPreviewPath()const { return mMosaicPreview.c_str();}

    void setTileSize(int tileSize);
    int getTileSize()const;

    void addTileImage(int width,
                        int height,
                        int components,
                        unsigned char *data,
                        const char *filepath,
                        TileId id);
    bool removeTileImage(TileId id);

    bool hasTileImage(TileId id)const;

    bool updateTileImage(int width,
                         int height,
                         int components,
                         unsigned char *data,
                         const char *filepath,
                         TileId id);

    bool generate(int width,
                  int height,
                  int components,
                  unsigned char *data);

    const char *getMosaicMap()const;
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
