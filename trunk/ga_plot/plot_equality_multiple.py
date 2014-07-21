'''
Plots the ammount of equal individuals along the generations
'''
import os
import sys
import glob
import numpy as np
import matplotlib.pyplot as plt

def do_plots(rootdirArray):

    dirArray = dict()

    for basedir in rootdirArray:
        dirArray[basedir] = dict()

        generations = len(glob.glob(os.path.join(basedir, '*')))

        print 'Found %d generations for %s .' %(generations, basedir)

        #counts the number of equal individuals along the generations
        for i in range(1, generations+1):
            equal_count = 0 #counts the number of equal individuals in this generations

            #retrieves all the chromosome files
            chromo_files = glob.glob(os.path.join(basedir, 'g%d' % i, '*.chr')) + glob.glob(os.path.join(basedir, 'g%d'  % i, '*.chr.lock'))
            #traverse the chromosome files, making pairwise comparison for equality of content
            for c1 in range(0, len(chromo_files) - 1):
                for c2 in range(c1+1, len(chromo_files)):
                    if open(chromo_files[c1]).read() == open(chromo_files[c2]).read():
                        equal_count += 1
            
            dirArray[basedir][i] = equal_count

            print 'Generation %d analysed, %d equal.' %(i, equal_count)

    '''
    # This is for DEBUG
    # It print all the mean fitness values for every folder in blue
    for folderName in rootdirArray:

        plotX = []
        plotY = []

        for key in dirArray[folderName].keys():
            plotX.append(key)
            plotY.append(dirArray[folderName][key])

        plt.plot(plotX, plotY, 'b', label='mean')
        plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))
    '''

    plotX = []
    plotY = []
    for key in dirArray[rootdirArray[0]].keys():
        plotX.append(key)
        
        meanValues = 0
        for folderName in rootdirArray:
            meanValues += dirArray[folderName][key]

        #print meanValues

        plotY.append(meanValues / len(rootdirArray))

    plt.plot(plotX, plotY, 'r', label='Mean')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))
    
    '''
    # Find the top bound for the Y axis in debug mode
    maxNumberOfEqualIndividual = 100
    for key in dirArray[rootdirArray[0]].keys():
        
        for folderName in rootdirArray:
            if(dirArray[folderName][key] > maxNumberOfEqualIndividual):
                maxNumberOfEqualIndividual = dirArray[folderName][key]
    '''

    # Find the top bound for the Y axis
    maxNumberOfEqualIndividual = 0
    for value in plotY:
        if(value > maxNumberOfEqualIndividual):
            maxNumberOfEqualIndividual = value

    plt.ylabel('Equal number of individuals')
    plt.xlabel('Generation')
    plt.axis([min(plotX), max(plotX), 0, maxNumberOfEqualIndividual+1])
    plt.legend(ncol=3)
    plt.show()

if __name__ == '__main__':
    if(len(sys.argv) <= 1):
        print "    There must be at least one folder path as argument"
        print "    Example: python plot_equality_multiple.py prot1 prot2 prot3"
    else:
        do_plots(sys.argv[1:])
