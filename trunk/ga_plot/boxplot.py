import numpy as np
import matplotlib.pyplot as plt
import os
import re
import sys

def do_plots(rootdir):

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

    #for key in fitnessDict.keys():
     #   plotX.append(key)
      #  plotY.append(np.mean())# sum(fitnessDict[key]) / float(len(fitnessDict[key])))
        #print key, '-->', sum(fitnessDict[key]) / float(len(fitnessDict[key]))

    last_gen_data = fitnessDict[max(fitnessDict.keys())]
    plt.boxplot(last_gen_data)#, 'b', label='mean')
    plt.plot(1, np.mean(last_gen_data), 'b*', ms=15)
    plt.ylabel('Fitness')
    plt.show()


    #plt.xlabel('Generation')
    #plt.axis([min(plotX), max(plotX), 0, 3])
    #plt.legend(ncol=3)
    plt.show()

if __name__ == '__main__':
    do_plots(sys.argv[1])