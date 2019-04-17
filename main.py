from modules import promethee_sort
import sys, os

def main():
    for option in sys.argv:
        if option == '-sort':
            promethee_sort.main(sys.argv[1:]) 


if __name__ == '__main__':
    main()