#ifndef MOSAICIMAGECREATOR_LIBRARY_H
#define MOSAICIMAGECREATOR_LIBRARY_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <cstdint>

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

class RegionOfInterest{
public:
    RegionOfInterest(){}

    RegionOfInterest(int xRegion, int yRegion, int widthRegion, int heightRegion) : x(xRegion), y(yRegion), width(widthRegion), height(heightRegion) {
    }

    // Parameterized constructor to create a square region centered within the image
    RegionOfInterest(int imageWidth, int imageHeight)
            : width(std::min(imageWidth, imageHeight)), height(std::min(imageWidth, imageHeight)) {
        // Calculate the center position
        x = (imageWidth - width) / 2;
        y = (imageHeight - height) / 2;
    }

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

class Mosaify {
public:
    typedef long long int TileId;

    typedef pair<unsigned int, unsigned int> Indices;

    typedef pair<TileId, NJLIC::Image*> TileImage;
    typedef pair<TileId, RegionOfInterest> TileROI;
    typedef pair<TileId, int> TileUse;

    typedef map<Indices, TileROI> MosaicMap;
    typedef pair<Indices, TileROI> MosaicMapPair;
private:
    vector<TileImage> mTileImages;
    vector<TileROI> mTileROIs;
    vector<TileUse> mTileUses;
    int mFinalMosaicScale;
    uint8 mTileSize;
    uint8 mPatchSize;
    int mMaxHeight;
    int mMaxWidth;

    MosaicMap mMosaicMap;
    NJLIC::Image *mMosaicImage;
    uint32_t mMaxTimesUsed;

    const NJLIC::Image *resizeImage(const NJLIC::Image *img)const;
public:
    int getMaxThreads()const;
    Mosaify();
    ~Mosaify();

    int getMaxWidth()const{return mMaxWidth;}
    int getMaxHeight()const{return mMaxHeight;}

    int getMaxTimesUsed()const{return mMaxTimesUsed;}
    void setMaxTimesUsed(const uint32_t m){mMaxTimesUsed = m;}


    void setFinalMosaicScale(int scale);
    int getFinalMosaicScale() const;

    void setTileSize(int tileSize);
    int getTileSize()const;

    void setPatchSize(int size);
    int getPatchSize()const;

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

    bool updateTileROI( int x, int y, int width, int height, TileId id);
    RegionOfInterest getTileROI( TileId id)const;
    void getTileROIs(vector<TileROI> &rois)const;

    void resetTileUse(TileId id);
    void increaseTileUse(TileId id);
    void decreaseTileUse(TileId id);
    int totalTileUse(TileId id)const;
    bool tileUsed(TileId id)const;

    bool generate(int width,
                  int height,
                  int components,
                  uint8 *data);

    void getMosaicImage(NJLIC::Image &img)const;

    const char *getMosaicMap()const;

    void getMosaicMap(MosaicMap &vec)const;
    static int scaleNumber(double scaleMax, double inputMax, double inputNumber) ;
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
