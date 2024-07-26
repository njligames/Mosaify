# project/test.py

import unittest
import glob
from PIL import Image as PILImage
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

    def test_tile_clip(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        _id = 1
        mosaify.addTile(_id, "input/tile.input/Photo on 9-7-18 at 9.09 AM #3.jpg")
        image = mosaify.getTileImage(1)
        image.show()

    def test_tile(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1
        image = mosaify.getTileImage(1)
        image.show()

    def test_generate_image_preview(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(1)
        mosaify.setPatchSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1
        with PILImage.open("input/target.jpg") as im:
            # Process the image here
            im = im.convert('RGB')
            data = im.tobytes()
            rows, cols = im.size
            comps = 3
            try:
                success = mosaify.generate(rows, cols, comps, data)
                self.assertTrue(success)
                image = mosaify.getMosaicPreviewImage()
                image.show()
            except RuntimeError as e:
                print("error")
                self.assertTrue(False)
            except:
                print("error")
                self.assertTrue(False)

    def test_generate_image(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        mosaify.setPatchSize(1)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1
        with PILImage.open("input/target.jpg") as im:
            # Process the image here
            im = im.convert('RGB')
            data = im.tobytes()
            rows, cols = im.size
            comps = 3
            try:
                success = mosaify.generate(rows, cols, comps, data)
                self.assertTrue(success)
                s = mosaify.getMosaicPath()
                print(s)
                self.assertTrue(True)
            except RuntimeError as e:
                print("error")
                self.assertTrue(False)
            except:
                print("error")
                self.assertTrue(False)

    def test_generate_map(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        mosaify.setPatchSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1
        with PILImage.open("input/target.jpg") as im:
            # Process the image here
            im = im.convert('RGB')
            data = im.tobytes()
            rows, cols = im.size
            comps = 3
            try:
                success = mosaify.generate(rows, cols, comps, data)
                self.assertTrue(success)
                s = mosaify.getMosaicMap()
                print(s)
                self.assertTrue(True)
            except RuntimeError as e:
                print("error")
                self.assertTrue(False)
            except:
                print("error")
                self.assertTrue(False)

    def test_generate_json(self):
        mosaify = MosaifyPy()
        mosaify.setTileSize(8)
        mosaify.setPatchSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1
        with PILImage.open("input/target.jpg") as im:
            # Process the image here
            im = im.convert('RGB')
            data = im.tobytes()
            rows, cols = im.size
            comps = 3
            try:
                success = mosaify.generate(rows, cols, comps, data)
                self.assertTrue(success)
                s = mosaify.getMosaicJsonArray()
                print(s)
                self.assertTrue(True)
            except RuntimeError as e:
                print("error")
                self.assertTrue(False)
            except:
                print("error")
                self.assertTrue(False)


if __name__ == '__main__':
    unittest.main()

