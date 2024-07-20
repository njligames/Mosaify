# project/test.py

import unittest
import glob
from PIL import Image
from MosaifyPy import Image
from MosaifyPy import MosaifyPy

class TestCalculations(unittest.TestCase):

    def test_image(self):
        img = Image()
        img.open("input/target.jpg")
        pilImg = img.toPILImage()
        pilImg.show()

    def test_image_clip(self):
        img = Image()
        img.open("input/target.jpg")
        img.clip(50, 50, 500, 500)
        pilImg = img.toPILImage()
        pilImg.show()

    def test_addtile(self):
        mosaify = MosaifyPy()

        _id = 1

        paths = glob.glob("input/tile.input/*.jpg")

        mosaify.addTile(_id, paths[0])

        self.assertTrue(mosaify.hasTile(_id))


    def test_addremovetile(self):
        mosaify = MosaifyPy()
        _id = 1

        paths = glob.glob("input/tile.input/*.jpg")

        mosaify.addTile(_id, paths[0])
        mosaify.removeTile(_id)

        self.assertFalse(mosaify.hasTile(_id))

    def test_invalidtileid(self):
        mosaify = MosaifyPy()
        _id = 1

        self.assertFalse(mosaify.hasTile(_id))

    def test_tilesize(self):
        mosaify = MosaifyPy()
        tileSize = 8

        mosaify.setTileSize(tileSize)

        self.assertEqual(tileSize, mosaify.getTileSize())

    # TODO: Test is broken
    def test_tile(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1

        image = mosaify.getTileImage(1)
        image.show()

    # def test_generate_image(self):
    #     mosaify = Mosaify.Mosaify()
    #     mosaify.setTileSize(8)
    #     _id = 1
    #     for path in glob.glob("input/tile.input/*.jpg"):
    #         mosaify.addTile(_id, path)
    #         _id = _id + 1

    #     with Image.open("input/target.jpg") as im:
    #         # Process the image here
    #         im = im.convert('RGB')
    #         data = im.tobytes()
    #         rows, cols = im.size
    #         comps = 3
    #         self.assertTrue(mosaify.generate(rows, cols, comps, data))

    #     # self.assertTrue(mosaify.generate("input/target.jpg"))
        
    #         print("The path to the mosaic Image", mosaify.getMosaicPath())

    #         image = mosaify.getMosaicPreviewImage()
    #         image.show()


    # def test_generate_map(self):
    #     mosaify = Mosaify.Mosaify()
    #     mosaify.setTileSize(8)
    #     _id = 1
    #     for path in glob.glob("input/tile.input/*.jpg"):
    #         mosaify.addTile(_id, path)
    #         _id = _id + 1

    #     # self.assertTrue(mosaify.generate("input/target.jpg"))

    #     # s = mosaify.getMosaicMap()
    #     # print(s)

    # def test_generate_json_array(self):
    #     mosaify = Mosaify.Mosaify()
    #     mosaify.setTileSize(8)
    #     _id = 1
    #     for path in glob.glob("input/tile.input/*.jpg"):
    #         mosaify.addTile(_id, path)
    #         _id = _id + 1

    #     # self.assertTrue(mosaify.generate("input/target.jpg"))

    #     # s = mosaify.getMosaicJsonArray()
    #     # print(s)

    # def test_generate_main_mosaic(self):
    #     mosaify = Mosaify.Mosaify()
    #     mosaify.setTileSize(8)
    #     _id = 1
    #     for path in glob.glob("input/tile.input/*.jpg"):
    #         mosaify.addTile(_id, path)
    #         _id = _id + 1

    #     # self.assertTrue(mosaify.generate("input/target.jpg"))

    #     # s = mosaify.getMosaicPath()
    #     # print(s)
        


if __name__ == '__main__':
    unittest.main()

