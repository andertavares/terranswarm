'''
Searches the individual with the best fitness in the last generation,
creates the c:\bestValues.txt file and run GAMedicReadValues_release.dll.

'''
import run_whole_experiment as rwe
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

    dest = sc_dir +  "\\bwapi-data" + "\\bestValues.txt"
    copy_dest = experiment_path + "\\bestValues.txt" #file containing the values from the best game

    if os.path.exists(copy_dest):
        best_file = copy_dest
        print 'Found bestValues.txt in %s, will use it.' % (experiment_path)
    else:
        best_file = best_fitness_file(experiment_path, cfg.function)
        shutil.copyfile(best_file, copy_dest)
        print '%s copied to %s, as a copy.' % (best_file, copy_dest)

    print '%s copied to %s, to be executed by MedicReadValues' % (best_file, dest)
    shutil.copyfile(best_file, dest)

    # puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_readValues_%s.ini' % enemy)
	
	# copies setup/GAMedicReadValues_release.dll to <starcraft>/bwapi-data/AI
    try:
        our_ai_dllpath = os.path.join('setup', 'GAMedicReadValues_release.dll')
        sc_ai_dllpath = os.path.join(sc_dir, 'bwapi-data', 'AI', 'GAMedicReadValues_release.dll')
        shutil.copyfile(our_ai_dllpath, sc_ai_dllpath)
    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
                'to %s' % (our_ai_dllpath, sc_ai_dllpath)
        exit()

    results_path = os.path.join(sc_dir, 'results.txt')
    open(results_path, 'w').close() #create empty results.txt file

    # calls chaoslauncher and monitors results.txt
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
        time.sleep(2)
        if not rwe.monitor_once(): #problem... relaunch starcraft
            print 'Removing <sc>/results.txt and restarting.'
            rfile.close()
            open(results_path, 'w').close()  # resets results.txt file
            chaosLauncher = subprocess.Popen([cl_path])
            last_read = 0
            #rwe.erase_best_values(sc_dir, current_experiment)
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


def best_fitness_file(experiment_path, fitness_function):
    """
    Function from ga_plot/get_best_fitness
    :param experiment_path:
    :param fitness_function: depending on the used function, must look for different .chr file
    :return:
    """

    rootdir = experiment_path

    fitnessDict = dict()

    for subdir, dirs, files in os.walk(rootdir):

        for fname in files:
            #print files
            fileName, fileExtension = os.path.splitext(fname)
            fullPath = os.path.join(subdir, fname)
            baseGeneration = os.path.basename(subdir)

            if baseGeneration != "":
                generationNumber = int(re.findall(r'\d+', baseGeneration)[0])

                if re.search(r'\d+\.fit$', fname) or re.search(r'\d+\.chr\.fit$', fname):
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

        for fname in files:
            fileName, fileExtension = os.path.splitext(fname)
            fullPath = os.path.join(subdir, fname)
            baseGeneration = os.path.basename(subdir)

            if re.search(r'\d+\.fit$', fname) or re.search(r'\d+\.chr\.fit$', fname):
                f = open(fullPath)
                fitness = float("".join(f.readlines()))
                f.close()
                if (fitness > bestFitnessValue):
                    bestFitnessValue = fitness
                    bestFitnessNumber = int(re.findall(r'\d+', fname)[0])
                    #print bestFitnessValue, bestFitnessNumber, file
                #print fitness, int(re.findall(r'\d+', file)[0]), fullPath

    bestFileName = str(fitnessDict[lastGeneration])+"/"+str(bestFitnessNumber)+".chr.lock"

    if fitness_function == configparser.ConfigParser.VICTORY_RATIO:
        bestFileName = os.path.join(str(fitnessDict[lastGeneration]), '%d-rep-0.chr.lock' % bestFitnessNumber)

    return bestFileName


if __name__ == '__main__':

    num_configs = len(sys.argv) - 1
    num_matches = int(sys.argv[len(sys.argv) - 1])

    print "%d" %num_configs
    print "%d" %num_matches
    print "%s" %sys.argv[1]

    for n in range (1, num_configs):
        go(sys.argv[n], num_matches)
        time.sleep(5)
