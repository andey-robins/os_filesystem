import os, sys

def main():
    ## validate command line args
    if len(sys.argv) == 1 or len(sys.argv) > 3:
        print("Usage: python bhelper.py <driver number> [<type of output>]")

    if not sys.argv[1] in range(1, 7):
        print("Please input a driver number between 1 and 6")

    if len(sys.argv) == 3 and (sys.argv[2] != 'f' or sys.argv[2] != 'c' or sys.argv[2] != 'a'):
        print("Please input a valid output type: f for failures, c for corrects, or a for all")

    driverNumber = sys.argv[1]
    outputType = 'a'

    if len(sys.argv) == 3:
        outputType = sys.argv[2]

    if outputType == 'a':
        outputType = '\"\"'
    elif outputType == 'f':
        outputType = 'fail\" | grep -v \"failed'
    else:
        outputType = 'correct'

    ## run the script
    os.system('make clean')
    os.system('make')

    for i in range(1, int(driverNumber) + 1):
        os.system('./drTester' + str(i) + ' | grep \"' + outputType + '\"')
    

if __name__ == "__main__":
    main()