import numpy as np
import matplotlib.pyplot as plt
import pprint
import glob
import os
import re
import sys

import argparse
import zipfile


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

    for key in fitnessDict.keys():
        plotX.append(key)
        plotY.append(np.mean(fitnessDict[key]))  # sum(fitnessDict[key]) / float(len(fitnessDict[key])))
        #print key, '-->', sum(fitnessDict[key]) / float(len(fitnessDict[key]))

    plt.plot(plotX, plotY, 'b', label='Terran')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    # plotX = []
    # plotY = []

    # for key in fitnessDict.keys():
    #     plotX.append(key)
    #     plotY.append(min(fitnessDict[key]))

    # plt.plot(plotX, plotY, 'r', label='min')
    # plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    # plotX = []
    # plotY = []

    # for key in fitnessDict.keys():
    #     plotX.append(key)
    #     plotY.append(max(fitnessDict[key]))

    # plt.plot(plotX, plotY, 'g', label='max')
    # plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plt.plot(plotX, [1] * len(plotX), 'y--')
    plt.ylabel('Fitness value')
    plt.xlabel('Generation')
    plt.axis([min(plotX), max(plotX), 0, 3])
    plt.legend(ncol=3)
    plt.grid(True)
    plt.show()



def get_mean_fitness_data(range_list, input_file, folder_name):
    #print 'gmfd with %s, %s, %s' % (range_list, input_file, folder_name)
    global number_of_generations
    #global number_of_population
    #global raw_exp_data

    #raw_exp_data[race_name] = {}

    #raw_exp_data = {}
    fit_across_generations = []

    for gen in range(1, number_of_generations + 1):

        fit_across_repetitions = []

        for rep in range_list:
            exp_data[int(rep)] = {}

            fit_pattern = os.path.join(input_file, folder_name + '%s' % rep, 'g%d' % gen, '*.fit')

            files = glob.glob(fit_pattern)
            sys.stdout.write('\rlooking at %s; found %3d items' % (fit_pattern, len(files)))
            fit_of_population = [float(open(f).read().strip()) for f in files]

            fit_across_repetitions.append(np.mean(fit_of_population))
            #raw_exp_data[int(rep)][gen] = np.mean(population_fitness)
            #print 'rep, gen, mean, data -- %d, %d, %f, %f' % (int(rep), gen, np.mean(population_fitness), raw_exp_data[int(rep)][gen])

        fit_across_generations.append(np.mean(fit_across_repetitions))

    #pprint.pprint(population_fitness)
    return fit_across_generations
'''
    for range_i in range_list:
        file_path = str(input_file) + str(folder_name) + range_i + '.zip'
        print file_path

        raw_exp_data[int(range_i)] = {}

        archive = zipfile.ZipFile(file_path, 'r')
        folder_list = list(set([os.path.split(x)[0] for x in archive.namelist() if '/' in x]))
        if number_of_generations == 0:
            number_of_generations = len(folder_list)
        if number_of_generations != len(folder_list):
            print "ERROR: the number of generations are not the same"
            sys.exit()

        for folder in folder_list:
            fit_path_list = list(set([os.path.split(x)[1] for x in archive.namelist() if folder_list[0] + '/' in x and x.endswith(".fit")]))
            if number_of_population == 0:
                number_of_population = len(fit_path_list)
            elif number_of_population != len(fit_path_list):
                print "ERROR: population size are not the same"
                sys.exit()

            generation_num = int(re.findall(r'\d+', folder)[0])
            fitness_list = []

            for fit in fit_path_list:
                fit_zip_data = archive.read(folder + "/" + fit)
                fit_data = float("".join(fit_zip_data))
                fitness_list.append(fit_data)

            raw_exp_data[int(range_i)][generation_num] = np.mean(fitness_list)

'''


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create plot of fitnes mean among generations from a set of experiments')

    parser.add_argument('-t', '--terran',   help="The terran source name", required=True)
    parser.add_argument('-z', '--zerg',     help="The zerg source name", required=True)
    parser.add_argument('-p', '--protoss',  help="The protoss source name", required=True)
    parser.add_argument('-r', '--range',   help="The source range e.g.: 1-10", required=True)
    parser.add_argument('-i', '--input',   help="The source dir", required=True)
    parser.add_argument('-g', '--generations',   help="The number of generations", required=True, type=int)

    args = parser.parse_args()

    number_of_generations = args.generations

    range_list = []
    range_limits = re.findall(r'\d+', args.range)

    number_of_zeroes = len(range_limits[0])
    format_zero_string = '0' + str(number_of_zeroes)

    range_limits = [int(s) for s in range_limits]

    if len(range_limits) != 2:
        print "Error, range is invalid"
        sys.exit()
    else:
        range_list = list(xrange(range_limits[0], range_limits[1] + 1))
        range_list = [format(n, format_zero_string) for n in range_list]

    input_folder = str(args.input)
    terran_folder = str(args.terran)
    zerg_folder = str(args.zerg)
    protoss_folder = str(args.protoss)

    print 'Parameters:'
    print '     -Terran name: "%s"' % terran_folder
    print '     -Zerg name: "%s"' % zerg_folder
    print '     -Protoss name: "%s"' % protoss_folder
    print '     -Range: "%s" ' % range_list
    print '     -Source dir: "%s"' % input_folder
    print  #skips a line

    #gathers experiment data
    exp_data = {}
    exp_data["terran"] = get_mean_fitness_data(range_list, input_folder, terran_folder)
    exp_data["zerg"] = get_mean_fitness_data(range_list, input_folder, zerg_folder)
    exp_data["protoss"] = get_mean_fitness_data(range_list, input_folder, protoss_folder)
    print '\rData gathered, generating plot...                            '

    plt.plot(range(1, len(exp_data["terran"]) +1), exp_data["terran"], 'b', label='vs Terran')
    plt.plot(range(1, len(exp_data["protoss"]) +1), exp_data["protoss"], 'g', label='vs Protoss')
    plt.plot(range(1, len(exp_data["zerg"]) +1), exp_data["zerg"], 'r', label='vs Zerg')

    #the baseline
    plt.plot(range(1, len(exp_data["zerg"]) +1), [1] * number_of_generations, 'y--')
    plt.xlim(1, len(exp_data["zerg"]))


    plt.legend(ncol=3)
    #plt.grid(True)
    print 'Done.'
    plt.show()
