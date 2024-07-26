/* File : MosaifyPy.i */
%module MosaifyPy

%begin %{
#pragma warning(disable:4100 4127 4706)
%}

%{
#include "Mosaify/Mosaify.h"
#include "Mosaify/Image.h"
#include "libboard/include/board/Globals.h"

#include <iostream>
using namespace std;

#include <Board.h>

const char * getMosaicPath(const Mosaify &mosaify){
    LibBoard::Board board;
    static string svg_path = string(std::getenv("TMPDIR") + string("/image_") + to_string(rand()) + string(".svg"));
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
        string path = string(std::getenv("TMPDIR") + string("/") + to_string(tid) + string(".png"));
        NJLIC::Image *img = new NJLIC::Image();
        mosaify.getTileImage(tid, *img);
        auto roi = mosaify.getTileROI(1);
        img->clip(glm::vec2(roi.x, roi.y), roi.width, roi.height);
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
//    static string path = string(fs::temp_directory_path()) + to_string(rand()) + string(".png");
    static string path = string(std::getenv("TMPDIR") + string("/") + to_string(rand()) + string(".png"));
    NJLIC::Image *img = new NJLIC::Image();
    mosaify.getMosaicImage(*img);
    ImageFileLoader::write(path, img);

    delete img;
    return path.c_str();
}

const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id){
    static string path = string(std::getenv("TMPDIR") + string("/") + to_string(rand()) + string(".png"));

    NJLIC::Image *img = new NJLIC::Image();
    if(mosaify.getTileImage(_id, *img)) {
        auto roi = mosaify.getTileROI(1);
        img->clip(glm::vec2(roi.x, roi.y), roi.width, roi.height);
        ImageFileLoader::write(path, img);
    }

    delete img;
    return path.c_str();
}
%}

%include <std_string.i>
%include <exception.i>

%exception {
    try {
        $action
    } catch(const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch(...) {
        SWIG_exception(SWIG_UnknownError, "");
    }
}

%typemap(in) uint8* (char* buffer, Py_ssize_t length) {
    PyBytes_AsStringAndSize($input,&buffer,&length);
    $1 = (uint8*)buffer;
}

%include "Mosaify/Mosaify.h"
%include "Mosaify/Image.h"
const char * getMosaicPath(const Mosaify &mosaify);
const char * getMosaicPreviewPath(const Mosaify &mosaify);
const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id);
