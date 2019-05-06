from modules import promethee_sort, promethee_calculate
import sys, os

def main():
    if sys.argv == '-sort':
        promethee_sort.sort(sys.argv[1:])
    elif sys.argv == '-reverse':
        promethee_sort.unsort(sys.argv[1:])
    else:
        promethee_sort.sort(sys.argv)

        promethee_calculate.calculate(sys.argv[1:])

        param = []
        param.append('-reverse')
        param.append('out.' + sys.argv[1])
        param.append('positions.tif')
        for i in xrange(2, len(sys.argv)):
            param.append(sys.argv[i])
        promethee_sort.unsort(param)


if __name__ == '__main__':
    main()