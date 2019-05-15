from modules import promethee_sort, promethee_calculate
import sys, os

def main():
    if '-sort' in sys.argv:
        promethee_sort.sort(sys.argv[1:])
    elif '-reverse' in sys.argv:
        promethee_sort.unsort(sys.argv[1:])
    elif '-fp' in sys.argv:
        promethee_sort.sort(sys.argv[1:])

        promethee_calculate.calculate(sys.argv[1:])

        param = []
        param.append('-reverse')
        os.system('mv out.* result_' + sys.argv[2])
        param.append('result_' + sys.argv[2])
        param.append('positions.tif')
        for i in xrange(3, len(sys.argv)):
            param.append(sys.argv[i])
        promethee_sort.unsort(param)
    else:
        promethee_calculate.calculate(sys.argv[1:])


if __name__ == '__main__':
    main()