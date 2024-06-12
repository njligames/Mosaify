# project/test.py

import unittest
import glob
import Mosaify

class TestCalculations(unittest.TestCase):

    def test_addtile(self):
        mosaify = Mosaify.Mosaify()
        _id = 1

        paths = glob.glob("input/tile.input/*.jpg")

        mosaify.addTile(_id, paths[0])

        self.assertTrue(mosaify.hasTile(_id))


    def test_addremovetile(self):
        mosaify = Mosaify.Mosaify()
        _id = 1

        paths = glob.glob("input/tile.input/*.jpg")

        mosaify.addTile(_id, paths[0])
        mosaify.removeTile(_id)

        self.assertFalse(mosaify.hasTile(_id))

    def test_invalidtileid(self):
        mosaify = Mosaify.Mosaify()
        _id = 1

        self.assertFalse(mosaify.hasTile(_id))

    def test_tilesize(self):
        mosaify = Mosaify.Mosaify()
        tileSize = 8

        mosaify.setTileSize(tileSize)

        self.assertEqual(tileSize, mosaify.getTileSize())

    def test_generate_image(self):
        mosaify = Mosaify.Mosaify()
        mosaify.setTileSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1

        self.assertTrue(mosaify.generate("input/target.jpg"))

        image = mosaify.getMosaicImage()
        image.show()

    def test_generate_map(self):
        mosaify = Mosaify.Mosaify()
        mosaify.setTileSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1

        self.assertTrue(mosaify.generate("input/target.jpg"))

        mosaicMap = mosaify.getMosaicMap()
        print(mosaicMap)
        


if __name__ == '__main__':
    unittest.main()

