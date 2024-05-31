# Python code example
import MosaifyPy
from PIL import Image
import glob
import sys

def pil_image(path):
    im = Image.open(path)
    im = im.convert('RGBA')

    si = MosaifyPy.simple_image()
    si.rows,si.cols = im.size
    s = im.tobytes() # Must keep a reference
    si.imgdata = s
    si.comp = 4
    return si


def getMinSize():
	minWidth = 0
	minHeight = 0
	for path in glob.glob("input/tile.input/*.jpg"):
		im = Image.open(path)
		width, height = im.size
		if width > minWidth:
			minWidth = width
		if height > minHeight:
			minHeight = height

	return (minWidth, minHeight)

def resize(size):
	i = 0
	for path in glob.glob("input/tile.input/*.jpg"):
		im = Image.open(path)
		width = size[0]
		height = size[1]


		im = im.resize(size)
		if width < height:
			im = im.crop((0, 0, width, width))
		else:
			im = im.crop((0, 0, height, height))
		im.save("input/tile.input.resize/" + str(i) + ".jpg") 
		i += 1

def mosaify():
	mosaic = MosaifyPy.Mosaify()
	mosaic.setTileSize(8)

	for path in glob.glob("input/tile.input.resize/*.jpg"):
		mosaic.addTileImage(pil_image(path), path)

	path = "input/target.jpg"
	if mosaic.generate(pil_image(path)):
		print(mosaic.createMontageCommand())
		# print(mosaic.getMosaicMap())
		# print("YESS")
	else:
	    print("Couldn't create the mosaic.")


size = getMinSize()
resize(size)
mosaify()