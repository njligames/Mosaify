/* File : MosaifyPy.i */
%module MosaifyPy

%begin %{
#pragma warning(disable:4100 4127 4706)
%}

%{
#include "Mosaify/Mosaify.h"
#include "Image.h"
#include "libboard/include/board/Globals.h"

#include <filesystem>
#include <iostream>
namespace fs = std::__fs::filesystem;
using namespace std;
#include <Board.h>

const char * getMosaicPath(const Mosaify &mosaify){

    LibBoard::Board board;
    static string svg_path = string(fs::temp_directory_path()) + string("image.svg");
    Mosaify::MosaicMap map;
    mosaify.getMosaicMap(map);
    int tileSize = 1;//mosaify.getTileSize();

    NJLIC::Image *mimg = new NJLIC::Image();
    mosaify.getMosaicImage(*mimg);

    int targetWidth = mimg->getWidth();
    int targetHeight = mimg->getHeight();
    int ncols = targetWidth / mosaify.getTileSize();
    int nrows = targetHeight / mosaify.getTileSize();

    delete mimg;

    for (const auto& pair : map) {
        Mosaify::Indices indices = pair.first;
        Mosaify::TileId tid = pair.second;

        string path = string(fs::temp_directory_path()) + to_string(tid) + string(".png");
        NJLIC::Image *img = new NJLIC::Image();
        mosaify.getTileImage(tid, *img);

        int dim = mosaify.getMaxWidth();
        if(dim > mosaify.getMaxHeight())
            dim = mosaify.getMaxHeight();
        img->resize(dim, dim);

        ImageFileLoader::write(path, img);
        delete img;

        int x = indices.first;
        int y = indices.second;
        LibBoard::Image image(path.c_str(), x * tileSize, (ncols - y) * tileSize, tileSize);
        board << image;
    }
    board.saveSVG(svg_path.c_str());
    return svg_path.c_str();
}

const char * getMosaicPreviewPath(const Mosaify &mosaify){
    static string path = string(fs::temp_directory_path()) + to_string(rand()) + string(".png");
    NJLIC::Image *img = new NJLIC::Image();
    mosaify.getMosaicImage(*img);
    ImageFileLoader::write(path, img);

    delete img;
    return path.c_str();
}

const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id){
    static string path = string(fs::temp_directory_path()) + to_string(rand()) + string(".png");
    NJLIC::Image *img = new NJLIC::Image();
    if(!mosaify.getTileImage(_id, *img)) {
       img->generate(mosaify.getTileSize(), mosaify.getTileSize(), 3);
    }
    ImageFileLoader::write(path, img);

    delete img;
    return path.c_str();
}
%}

%typemap(in) uint8* (char* buffer, Py_ssize_t length) {
    PyBytes_AsStringAndSize($input,&buffer,&length);
    $1 = (uint8*)buffer;
}

%include "Mosaify/Mosaify.h"
const char * getMosaicPath(const Mosaify &mosaify);
const char * getMosaicPreviewPath(const Mosaify &mosaify);
const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id);
