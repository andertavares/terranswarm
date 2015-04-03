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
	copy_dest = experiment_path + "\\bestValues.txt" #file containing the values from the best game
    shutil.copyfile(best_file, dest)
    shutil.copyfile(best_file, copy)
    print '%s copied to %s, to be executed by MedicReadValues' % (best_file, dest)
    print '%s copied to %s, as a copy.' % (best_file, dest)

    #puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_readValues_%s.ini' % enemy)

    results_path = os.path.join(sc_dir, 'results.txt')
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
    rfile.close()

    #checks if result.txt file exists:
    new_results_path = os.path.join(experiment_path, 'results.txt')

    if os.path.exists(new_results_path):

        i = 1
        while True:
            tentative_bkp_name = 'results_old%d.txt' % i
            if not os.path.exists(os.path.join(experiment_path, tentative_bkp_name)):
                break
            i += 1

        print '\nWARNING: a previous results.txt exists. It will be renamed to %s.\n' \
              'A new results.txt will be created with the results will soon appear below.\n' % tentative_bkp_name
        os.rename(new_results_path, os.path.join(experiment_path, tentative_bkp_name)) #renames the old results.txt file
    
    os.rename(results_path, new_results_path) #moves results.txt from Starcraft directory to experiment directory
     
    print 'Matches finished. Check the results in %s file.' % new_results_path


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
    num_matches = sys.argv[2] if len(sys.argv) > 2 else 30

    go(cfg_file, int(num_matches))
