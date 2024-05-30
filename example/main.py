# Python code example
import MosaifyPy
from PIL import Image
import glob

def pil_image(path):
    im = Image.open(path)
    im = im.convert('RGBA')

    si = MosaifyPy.simple_image()
    si.rows,si.cols = im.size
    s = im.tobytes() # Must keep a reference
    si.imgdata = s
    si.comp = 4
    return si

mosaic = MosaifyPy.Mosaify()
mosaic.setTileSize(8)

for path in glob.glob("input/tile.input/*.jpg"):
	print(path)
	mosaic.addTileImage(pil_image(path), path)

path = "input/target.jpg"
if mosaic.generate(pil_image(path)):
    print("yes")
else:
    print("no")

print(mosaic.getMosaicMap())
