import numpy as np
import matplotlib.pyplot as plt
import os
import re
import sys

def do_plots(rootdir):

    reliabilityDict = dict()

    for subdir, dirs, files in os.walk(rootdir):

        #print "Actual dir:", subdir

        for file in files:
            fileName, fileExtension = os.path.splitext(file)
            fullPath = os.path.join(subdir, file)
            baseGeneration = os.path.basename(subdir)

            if baseGeneration != "":
                generationNumber = int(re.findall(r'\d+', baseGeneration)[0])

                # Reliability values
                if fileExtension == ".misc" :
					f = open(fullPath)
					reliability = float("".join(f.readlines()))
					f.close()

					if generationNumber not in reliabilityDict:
						reliabilityDict[generationNumber] = []
					
					reliabilityDict[generationNumber].append(reliability)

    '''
    The dict reliabilityDict uses the integer number of the generation as the key
    to hold an array of values (the float values of the reliability).
    '''
    plotX = []
    plotY = []

    for key in reliabilityDict.keys():
        plotX.append(key)
        plotY.append(np.mean(reliabilityDict[key]))

    plt.plot(plotX, plotY, 'b*-')
    plt.ylabel('Reliability value (mean)')
    plt.xlabel('Generation Number')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))
    
    plotX = []
    plotY = []

    for key in reliabilityDict.keys():
        plotX.append(key)
        plotY.append(min(reliabilityDict[key]))

    plt.plot(plotX, plotY, 'r*-')
    plt.ylabel('Reliability value (min)')
    plt.xlabel('Generation Number')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plotX = []
    plotY = []

    for key in reliabilityDict.keys():
        plotX.append(key)
        plotY.append(max(reliabilityDict[key]))

    plt.plot(plotX, plotY, 'g*-')
    plt.ylabel('Reliability value (max)')
    plt.xlabel('Generation Number')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plt.show()

if __name__ == '__main__':
    do_plots(sys.argv[1])