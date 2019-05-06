from osgeo import gdal
import os

def sort(argument):
    argument = list(argument)
    dataset = gdal.Open(argument[1])
    width, height =  int(dataset.RasterXSize), int(dataset.RasterYSize)
    dataset = None
    partialPaths = []
    threads = 1
    bucket_size = 0
    for option in argument:
        if '-size=' in option:
            byte = int(option[6:])
            byte = byte * 512 * 1024
            total_buckets = (width * height * 8) / byte
            bucket_size = height / total_buckets
        if '-threads=' in option:
            threads = int(option[9:])

    if bucket_size == 0:
        print 'Size of sort invalid.'
        return 1
    
    start = 0
    xargs_argument = open('arguments', 'w')
    
    while start < height:
        line = ' '.join([
            '-sort', argument[1],
            '-start=' + str(start), '-end=' + str(min(start + bucket_size, height))
        ]) + '\n'
        partialPaths.append( 
            'values.' + str(start) + '-' + str(min(start + bucket_size, height)) + '.' + argument[1] + ' ' +
            'positions.' + str(start) + '-' + str(min(start + bucket_size, height)) + '.' + argument[1]                    
         )
        xargs_argument.write(line)
        start = start + bucket_size
    xargs_argument = None
    os.system('cat arguments | xargs -P ' + str(threads) + ' -n 4 ./run')
    a = ' '.join(['./run', '-sort', argument[1], 'positions.tif', '-kway '])
    b = ' '.join(partialPaths)
    os.system(a + b)

def unsort(argument):
    dataset = gdal.Open(argument[1])
    width, height =  int(dataset.RasterXSize), int(dataset.RasterYSize)
    dataset = None
    partialPaths = []
    threads = 1
    bucket_size = 0
    for option in argument:
        if '-size=' in option:
            byte = int(option[6:])
            byte = byte * 512 * 1024
            total_buckets = (width * height * 8) / byte
            bucket_size = height / total_buckets
        if '-threads=' in option:
            threads = int(option[9:])

    if bucket_size == 0:
        print 'Size of sort invalid.'
        return 1
    
    start = 0
    xargs_argument = open('arguments', 'w')
    
    while start < height:
        line = ' '.join([
            '-sort', '-reverse', argument[1], argument[2],
            '-start=' + str(start), '-end=' + str(min(start + bucket_size, height))
        ]) + '\n'
        partialPaths.append( 
            'values.' + str(start) + '-' + str(min(start + bucket_size, height)) + '.' + argument[1] + ' ' +
            'positions.' + str(start) + '-' + str(min(start + bucket_size, height)) + '.' + argument[1]                    
         )
        xargs_argument.write(line)
        start = start + bucket_size
    xargs_argument = None
    os.system('cat arguments | xargs -P ' + str(threads) + ' -n 6 ./run')
    a = ' '.join(['./run', '-sort','positions.tif', argument[1], '-kway '])
    b = ' '.join(partialPaths)
    os.system(a + b)