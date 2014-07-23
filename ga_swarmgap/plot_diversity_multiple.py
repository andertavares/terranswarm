'''
Plots a measure of diversity of the population along the generations.

First, consider a similarity index for one individual (a) compared to the rest of the population (P - {a}):

sim(a) = sum_{b \in P - {a}} sim(a,b)

where sim(a,b) is the similarity (implemented in ga_manager) between a and b. Now, the mean
similarity index across the population is given by:

sim(P) = sum_{a \in P} sim(a) / |P|

This script plots sim(P) along the generations of the GA

'''

import os
import re
import sys
import ga_manager
import numpy as np
import chromosome as chromo
import matplotlib.pyplot as plt

def similarity_index(chr, population):
    '''
    Calculates the similarity index for a chromosome to the population it belongs
    :param chr:
    :return:
    '''
    return sum( [1 - ga_manager.similarity(chr, other) for other in population if not chr == other] )


def do_plots(dir_array):
    #parallel_universes is a list of dicts, each dict stores information about a different experiment
    #palallel_universes will contain the different 'histories of mankind' ;)
    parallel_universes = []

    print 'Collecting chromosome data.'
    for rootdir in dir_array:
        # history_of_mankind is a dict, key is the generation number and value is an array of chromosomes
        history_of_mankind = dict()
        for subdir, dirs, files in os.walk(rootdir):

            #print "Actual dir:", subdir

            for file in files:
                fileName, fileExtension = os.path.splitext(file)
                fullPath = os.path.join(subdir, file)
                baseGeneration = os.path.basename(subdir)

                if baseGeneration != "":
                    generationNumber = int(re.findall(r'\d+', baseGeneration)[0])

                    if fileExtension == ".chr" or fileExtension == ".lock":
                        chr = {'chromosome': chromo.Chromosome.from_file(fullPath)}

                            #print '\t', generationNumber, file , fitness

                        if generationNumber not in history_of_mankind:
                            history_of_mankind[generationNumber] = []

                        history_of_mankind[generationNumber].append(chr)
        parallel_universes.append(history_of_mankind)
        print '%s data collected' % rootdir

    plotX = []
    plotY = []

    print 'Data from %d generations are to be averaged accross %d experiments' % (len(parallel_universes[0].keys()), len(parallel_universes))

    for generation in parallel_universes[0].keys():
        populations = [history_of_mankind[generation] for history_of_mankind in parallel_universes]

        plotX.append(generation)
        #point in Y axis is the mean of similarity index accross the same generation in different experiments
        plotY.append( np.mean([np.mean([similarity_index(c, population) for c in population]) for population in populations]) )
        sys.stdout.write('\rGeneration %d analysed.' % generation)

    plt.plot(plotX, plotY, 'b', label='mean sim')
    plt.ylabel('PDI')
    plt.xlabel('Generation')
    plt.show()
    print '\nDONE.'
    #plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))


if __name__ == '__main__':
    do_plots(sys.argv[1:])