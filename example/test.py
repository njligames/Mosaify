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

    def test_generate(self):
        mosaify = Mosaify.Mosaify()
        mosaify.setTileSize(8)
        _id = 1
        for path in glob.glob("input/tile.input/*.jpg"):
            mosaify.addTile(_id, path)
            _id = _id + 1

        self.assertTrue(mosaify.generate("input/target.jpg"))


if __name__ == '__main__':
    unittest.main()

