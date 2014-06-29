import numpy as np
import matplotlib.pyplot as plt
import os
import re
import sys

def do_plots(rootdir):

    #rootdir = 'test/'

    fitnessDict = dict()

    for subdir, dirs, files in os.walk(rootdir):

        #print "Actual dir:", subdir

        for file in files:
            fileName, fileExtension = os.path.splitext(file)
            fullPath = os.path.join(subdir, file)
            baseGeneration = os.path.basename(subdir)

            if baseGeneration != "":
                generationNumber = int(re.findall(r'\d+', baseGeneration)[0])

                if fileExtension == ".fit" :
                    f = open(fullPath)
                    fitness = float("".join(f.readlines()))
                    f.close()
                        #print '\t', generationNumber, file , fitness

                    if generationNumber not in fitnessDict:
                        fitnessDict[generationNumber] = []

                    fitnessDict[generationNumber].append(fitness)

    '''
    The dict fitnessDict uses the integer number of the generation as the key
    to hold an array of values (the float values of the fitness).
    '''
    plotX = []
    plotY = []

    for key in fitnessDict.keys():
        plotX.append(key)
        plotY.append(np.mean(fitnessDict[key]))# sum(fitnessDict[key]) / float(len(fitnessDict[key])))
        #print key, '-->', sum(fitnessDict[key]) / float(len(fitnessDict[key]))

    plt.plot(plotX, plotY, 'b', label='mean')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))


    plotX = []
    plotY = []

    for key in fitnessDict.keys():
        plotX.append(key)
        plotY.append(min(fitnessDict[key]))

    plt.plot(plotX, plotY, 'r', label='min')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plotX = []
    plotY = []

    for key in fitnessDict.keys():
        plotX.append(key)
        plotY.append(max(fitnessDict[key]))

    plt.plot(plotX, plotY, 'g', label='max')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plt.ylabel('Fitness value')
    plt.xlabel('Generation')
    plt.axis([min(plotX), max(plotX), 0, 160000])
    plt.legend(ncol=3)
    plt.show()

if __name__ == '__main__':
    do_plots(sys.argv[1])