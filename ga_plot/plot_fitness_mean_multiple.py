import numpy as np
import matplotlib.pyplot as plt
import os
import re
import sys

def do_plots(rootdirArray):
    '''
    The dict dirArray contains a entry of every folder and inside it there is another
    dict (the previous fitnessDict) that uses the integer number of the generation as the key
    to hold an array of values (the float values of the fitness).
    '''

    dirArray = dict()
    for folderName in rootdirArray:

        #print "Working on:", folderName

        dirArray[folderName] = dict()
    
        for subdir, dirs, files in os.walk(folderName):

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

                        if generationNumber not in dirArray[folderName]:
                            dirArray[folderName][generationNumber] = []

                        dirArray[folderName][generationNumber].append(fitness)

    print dirArray

    '''
    # This is for DEBUG
    # It print all the mean fitness values for every folder in blue
    for folderName in rootdirArray:

        plotX = []
        plotY = []

        for key in dirArray[folderName].keys():
            plotX.append(key)
            plotY.append(np.mean(dirArray[folderName][key]))

        plt.plot(plotX, plotY, 'b', label='mean')
        plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))
    '''

    plotX = []
    plotY = []
    for key in dirArray[rootdirArray[0]].keys():
        plotX.append(key)
        
        meanValues = 0
        for folderName in rootdirArray:
            meanValues += np.mean(dirArray[folderName][key])

        plotY.append(meanValues / len(rootdirArray))

    plt.plot(plotX, plotY, 'r', label='Mean')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plt.plot(plotX, [1] * len(plotX),'y--')#,label='equal')
    plt.ylabel('Fitness value')
    plt.xlabel('Generation')
    plt.axis([min(plotX), max(plotX), 0, 3])
    plt.legend(ncol=3)
    plt.show()

if __name__ == '__main__':
    if(len(sys.argv) <= 1):
        print "    There must be at least one folder path as argument"
        print "    Example: python prot_fitness_mean_multiple.py prot1 prot2 prot3"
    else:
        do_plots(sys.argv[1:])