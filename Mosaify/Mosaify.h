#ifndef MOSAICIMAGECREATOR_LIBRARY_H
#define MOSAICIMAGECREATOR_LIBRARY_H

typedef unsigned int uint32;
typedef unsigned char uint8;

typedef struct simple_image_t {
    uint32 rows;
    uint32 cols;
    uint8 *imgdata;
} simple_image;

class Mosaify {
public:
    Mosaify();

    void setTileSize(int tileSize);

    void addTileImage(int width,
                      int height,
                      int components,
                      unsigned char *data);
    void addTileImage(const char *file);
    void addTileImage(const simple_image* si);

    bool generate(int width,
                  int height,
                  int components,
                  unsigned char *data);
    bool generate(const char *file);
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
