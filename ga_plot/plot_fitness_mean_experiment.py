import numpy as np
import matplotlib.pyplot as plt
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
    global number_of_generations
    global number_of_population
    #global raw_exp_data

    #raw_exp_data[race_name] = {}

    raw_exp_data = {}

    for range_i in range_list:
        file_path = str(input_file) + str(folder_name) + range_i + '.zip'
        print file_path

        raw_exp_data[int(range_i)] = {}

        archive = zipfile.ZipFile(file_path, 'r')
        folder_list = list(set([os.path.split(x)[0] for x in archive.namelist() if '/' in x]))
        if number_of_generations == 0:
            number_of_generations = len(folder_list)
        if number_of_generations != len(folder_list):
            print "ERROR: the number of generations are not the same: %d in number vs %d in folder" % (number_of_generations, len(folder_list))
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
                try:
                    fit_zip_data = archive.read(folder + "/" + fit)
                    fit_data = float("".join(fit_zip_data))
                    fitness_list.append(fit_data)
                except Exception as e:
                    print 'An error occurred: %s' % e
                    print 'But we will skip the file that caused the error and proceed'


            raw_exp_data[int(range_i)][generation_num] = np.mean(fitness_list)

    return raw_exp_data


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Create plot of fitnes mean among generations from a set of experiments')

    parser.add_argument('-t', '--terran',   help="The terran source name", required=True)
    parser.add_argument('-z', '--zerg',     help="The zerg source name", required=True)
    parser.add_argument('-p', '--protoss',  help="The protoss source name", required=True)
    parser.add_argument('-r', '--range',   help="The source range Ej: 1-10", required=True)
    parser.add_argument('-i', '--input',   help="The source dir", required=True)

    args = parser.parse_args()

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
    print '     -Terran name:', "'" + terran_folder + "'"
    print '     -Zerg name:', "'" + zerg_folder + "'"
    print '     -Protoss name:', "'" + protoss_folder + "'"
    print '     -Range:', "'" + str(range_list) + "'"
    print '     -Source dir:', "'" + input_folder + "'\n"

    number_of_generations = 0
    number_of_population = 0

    raw_exp_data = {}
    raw_exp_data["terran"] = get_mean_fitness_data(range_list, input_folder, terran_folder)
    raw_exp_data["zerg"] = get_mean_fitness_data(range_list, input_folder, zerg_folder)
    raw_exp_data["protoss"] = get_mean_fitness_data(range_list, input_folder, protoss_folder)

    print "Generations number:", number_of_generations
    print "Population number:", number_of_population
    #print raw_exp_data

    plotX = []
    plotY = []

    for g in xrange(1, number_of_generations + 1):
        plotX.append(g)
        gen_values = []
        for i in xrange(int(range_list[0]), int(range_list[1]) + 1):
            gen_values = raw_exp_data["terran"][i][g]
        plotY.append(np.mean(gen_values))

    plt.plot(plotX, plotY, 'b', label='Terran')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plotX = []
    plotY = []

    for g in xrange(1, number_of_generations + 1):
        plotX.append(g)
        gen_values = []
        for i in xrange(int(range_list[0]), int(range_list[1]) + 1):
            gen_values = raw_exp_data["zerg"][i][g]
        plotY.append(np.mean(gen_values))

    plt.plot(plotX, plotY, 'r', label='zerg')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plotX = []
    plotY = []

    for g in xrange(1, number_of_generations + 1):
        plotX.append(g)
        gen_values = []
        for i in xrange(int(range_list[0]), int(range_list[1]) + 1):
            gen_values = raw_exp_data["protoss"][i][g]
        plotY.append(np.mean(gen_values))

    plt.plot(plotX, plotY, 'g', label='protoss')
    plt.xticks(np.arange(min(plotX), max(plotX)+1, 2.0))

    plt.plot(plotX, [1] * len(plotX), 'y--')
    plt.ylabel('Mean Time based fitness')
    plt.xlabel('Generation')
    plt.axis([min(plotX), max(plotX), 0, 1])
    plt.legend(ncol=3)
    plt.grid(True)
    plt.show()
