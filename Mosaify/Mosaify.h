#ifndef MOSAICIMAGECREATOR_LIBRARY_H
#define MOSAICIMAGECREATOR_LIBRARY_H

class Mosaify {
public:
    Mosaify();

//    # Python code example
//    import MosaifyPy
//    t = MosaifyPy.Mosaify()
//    t.setTileSize(8)

    // Declare copy constructor and assignment operator as deleted to prevent copying
//    Mosaify(const Mosaify&) = delete;
//    Mosaify& operator=(const Mosaify&) = delete;

    void setTileSize(int tileSize);

    void addTileImage(int width,
                      int height,
                      int components,
                      unsigned char *data);
    void addTileImage(const char *file);

    bool generate(int width,
                  int height,
                  int components,
                  unsigned char *data);
    bool generate(const char *file);
};

#endif //MOSAICIMAGECREATOR_LIBRARY_H
