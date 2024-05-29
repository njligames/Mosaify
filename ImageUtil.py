# Python code example

from PIL import Image


def pil_image(path):
    im = Image.open(path)
    im = im.convert('L')
    si = MosaifyPy.simple_image()
    si.rows,si.cols = im.size
    s = im.tobytes() # Must keep a reference
    si.imgdata = s
    return si
