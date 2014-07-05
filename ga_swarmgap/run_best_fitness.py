'''
Searches the individual with the best fitness in the last generation,
creates the c:\bestValues.txt file and run GAMedicReadValues_release.dll.

'''
import configparser
import subprocess
import shutil
import paths
import time
import sys
import os
import re

def go(cfg_file, num_matches):
    sc_dir, cl_path = paths.read_paths()
    cfg = configparser.ConfigParser(cfg_file)

    enemy = 'protoss'
    if cfg.enemy is None:
        print 'WARNING! Enemy not defined in .xml file, assuming protoss.'
    else:
        enemy = cfg.enemy

    experiment_path = os.path.join(sc_dir, cfg.output_dir)

    best_file = best_fitness_file(experiment_path)

    dest = "c:/bestValues.txt"
    shutil.copyfile(best_file, dest)
    print '%s copied to %s' % (best_file, dest)

    #puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_readValues_%s.ini' % enemy)

    #checks if result.txt file exists:
    results_path = os.path.join(sc_dir, 'results.txt')

    if os.path.exists(results_path):

        print 'WARNING: a previous results.txt exists. It will be renamed to results_old.txt.\n' \
              'A new results.txt will be created with the results will soon appear below.'
        os.rename(results_path, os.path.join(sc_dir, 'results_old.txt'))
        open(results_path, 'w').close() #create empty results.txt file

    #calls chaoslauncher and monitors results.txt
    chaosLauncher = subprocess.Popen([cl_path])
    last_read = 0
    while True:
        rfile = open(results_path)
        res_lines = rfile.readlines()

        if len(res_lines) > last_read:
            print 'Result for match %d: %s' % (len(res_lines), res_lines[-1].strip())
            last_read = len(res_lines)

        if len(res_lines) >= num_matches:
            break
        time.sleep(1)
        #print len(res_lines)
    #terminates chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    chaosLauncher.terminate()

    print 'Matches finished. Check the results in %s file.' % results_path


def best_fitness_file(experiment_path):
    '''
    Function from ga_plot/get_best_fitness
    :param experiment_path:
    :return:
    '''

    rootdir = experiment_path

    fitnessDict = dict()

    for subdir, dirs, files in os.walk(rootdir):

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

                        fitnessDict[generationNumber] = subdir

    '''
    The dict fitnessDict uses the integer number of the generation as the key
    to hold an array of values (the float values of the fitness).
    '''

    lastGeneration = fitnessDict.keys()[len(fitnessDict.keys()) -1]

    # TODO: Optimize for less uglyness

    bestFitnessValue = 0
    bestFitnessFile = None
    bestFitnessNumber = None

    for subdir, dirs, files in os.walk(fitnessDict[lastGeneration]):

        for file in files:
            fileName, fileExtension = os.path.splitext(file)
            fullPath = os.path.join(subdir, file)
            baseGeneration = os.path.basename(subdir)

            if fileExtension == ".fit" :
                f = open(fullPath)
                fitness = float("".join(f.readlines()))
                f.close()
                if (fitness > bestFitnessValue):
                    bestFitnessValue = fitness
                    bestFitnessNumber = int(re.findall(r'\d+', file)[0])
                    #print bestFitnessValue, bestFitnessNumber, file
                #print fitness, int(re.findall(r'\d+', file)[0]), fullPath

    bestFileName = str(fitnessDict[lastGeneration])+"/"+str(bestFitnessNumber)+".chr.lock"
    return bestFileName


if __name__ == '__main__':
    cfg_file = sys.argv[1]
    num_matches = sys.argv[2] if len(sys.argv) > 1 else 30

    go(cfg_file, int(num_matches))