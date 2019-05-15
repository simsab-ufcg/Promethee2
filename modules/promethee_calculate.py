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
     if not hasFlag(args, '-size') and hasFlag(args, '-fp'):
         args.append('-size=1024')

     if not hasFlag(args, '-type') and (hasFlag(args,'-um') or hasFlag(args, '-fp')):
        raise ValueError("Preference function must be specified")
     os.system(' '.join(['./run'] + args))