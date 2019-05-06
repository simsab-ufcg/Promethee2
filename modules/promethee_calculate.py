from osgeo import gdal
import os

def hasFlag(args, key):
    founded = False
    for arg in args:
        if key in arg:
            founded = True
            break
    return founded

def calculate(args):
     args = list(args)
     if not hasFlag(args, '-size'):
         args.append('-size=1024')

     if not hasFlag(args, '-type'):
        raise ValueError("Preference function must be specified")
     os.system(' '.join(['./run'] + args))