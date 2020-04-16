import os
import re
import sys
import shutil
import distutils.dir_util
import random
import copy
import paths
import geneticoperators as genops
import chromosome as chromo
import subprocess
import time
import glob
import xml
import run_whole_experiment as rwe

import configparser
from chromosome import Chromosome

def estimate_fitness(c, p1, p2):
    """
    Estimates fitness of a child (c), based on the parents (p1, p2) data
    :param c: the child
    :param p1: a parent
    :param p2: the other parent
    :return: the estimated fitness

    """

    #creates aliases for the variables that enter the equation
    s1, s2 = similarity(p1, c), similarity(p2, c)
    r1, r2 = p1['reliability'], p2['reliability']
    f1, f2 = p1['fitness'], p2['fitness']

    return ((r1 * f1 * s1) + (r2 * f2 * s2)) / float((r1 * s1) + (r2 * s2))


def reliability(c, p1, p2):
    """
    Returns the reliability of a child given its parents
    :param c: the child
    :param p1: a parent
    :param p2: the other parent
    :return: the calculated reliability

    """

    # calculates the similarities between the child and the two parents
    s1, s2 = similarity(p1, c), similarity(p2, c)
    r1, r2 = p1['reliability'], p2['reliability']

    # uses reliability equation to calculate reliability
    return ((s1*r1)**2 + (s2*r2)**2) / float(s1*r1 + s2*r2)


def similarity(child, parent):
    """
    Returns the similarity between a child and a parent
    :param child:
    :param parent:
    :return: the similarity value in [0..1]

    """
    chr_length = child['chromosome'].size

    parent_array = parent['chromosome']._genes
    child_array = child['chromosome']._genes

    partial = 0.0
    for i in range(0, chr_length):
        index = child['chromosome'].GENE_NAMES[i]
        partial += abs(parent_array[index].value - child_array[index].value) / \
                   abs(float(child_array[index].domain.max_value() - child_array[index].domain.min_value()))

    return 1 - (partial / chr_length)


def start(cfg):
    """
    Starts the genetic algorithm with configurations given by cfg object
    :param cfg: an instance of ConfigParser
    :return:
    """

    random.seed(cfg.random_seed)

    enemy = 'protoss'
    if cfg.enemy is None:
        print "WARNING: enemy race not specified. Assuming 'Protoss'"
    else:
        enemy = cfg.enemy

    # decides which function will be called to evaluate the population
    eval_population_function = evaluate
    if cfg.function == cfg.VICTORY_RATIO:
        eval_population_function = evaluate_victory_ratio

    # copies bwapi.ini from experiment dir to starcraft dir
    sc_dir, cl_path = paths.read_paths()
    try:
        exp_inipath = os.path.join('setup', 'bwapi_%s.ini' % enemy)
        sc_inipath = os.path.join(sc_dir, 'bwapi-data', 'bwapi.ini')
        shutil.copyfile(exp_inipath, sc_inipath)
    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
              'to %s' % (exp_inipath, sc_inipath)
        exit()

    # replaces BWAPI.dll with our hacked with enemy score retrieval
    try:
        our_dllpath = os.path.join('setup', 'BWAPI.dll')
        sc_dllpath = os.path.join(sc_dir, 'bwapi-data', 'BWAPI.dll')
        shutil.copyfile(our_dllpath, sc_dllpath)
    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
                'to %s' % (our_dllpath, sc_dllpath)
        exit()
		 
	# copies GAMedicAIModule_release.dll to <starcraft>/bwapi-data/AI
    try:
        our_ai_dllpath = os.path.join('setup', 'GAMedicAIModule_release.dll')
        sc_ai_dllpath = os.path.join(sc_dir, 'bwapi-data', 'AI', 'GAMedicAIModule_release.dll')
        shutil.copyfile(our_ai_dllpath, sc_ai_dllpath)
    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
                'to %s' % (our_ai_dllpath, sc_ai_dllpath)
        exit()

    # generates initial population
    old_pop = []
    for p in range(0, cfg.popsize):
        # sets fitness to 1 because population will be evaluated right after this loop
        old_pop.append({'chromosome': Chromosome(), 'fitness': 1, 'reliability': 0})

    # evaluates the 1st generation
    print 'Evaluating generation #1 of %s' % cfg.output_dir
    eval_population_function(old_pop, 1, cfg)

    for i in range(1, cfg.generations):  # starts from 1 because 1st generation (index 0) was evaluated already
        new_pop = []

        if cfg.elitism:
            # adds the best individual from previous population
            new_pop.append(genops.elite(old_pop))

        while len(new_pop) < cfg.popsize:
            (p1, p2) = genops.tournament_selection(old_pop, cfg.tournament_size)
            (c1, c2) = genops.crossover(p1, p2, cfg.p_crossover)
            genops.mutation(c1, cfg.p_mutation)
            genops.mutation(c2, cfg.p_mutation)

            # estimates fitness of children
            c1['fitness'] = estimate_fitness(c1, p1, p2)
            c2['fitness'] = estimate_fitness(c2, p1, p2)

            # calculates reliability of children
            c1['reliability'] = reliability(c1, p1, p2)
            c2['reliability'] = reliability(c2, p1, p2)

            # adds children to the new population
            # because of elitism, we test if second child can be added
            # otherwise, new population size can be 1 + old pop. size
            new_pop.append(c1)
            if len(new_pop) < cfg.popsize:
                new_pop.append(c2)

        # new population built, now evaluates it. Generation number is i+1
        # evaluate means calling the population evaluation function
        print '\nEvaluating generation #%d of %s' % ((i+1), cfg.output_dir)
        #evaluate(new_pop, i+1, cfg)
        eval_population_function(new_pop, i+1, cfg)

        old_pop = new_pop  # prepares for the next generation


def score_fit(xml_file):
    return float(xml_file.find('scoreRatio').get('value'))


def time_fit(xml_file):
    """
    Fitness based on match duration (assuming max duration of 3600 sec.)
    :param xml_file:
    :return:
    """
    result = xml_file.find('result').get('value')
    duration = int(xml_file.find('gameDuration').get('value'))

    time_fitness = 0.0
    if result == 'win':
        time_fitness = 1.0 - (duration / 7200.0)
    else:
        time_fitness = duration / 7200.0

    return time_fitness


def unit_fit(xml_file):
    unitsAvg = int(xml_file.find('unitsAverage').get('value'))
    return float(unitsAvg / 130.0)


def unit_score_fit(xml_file):
    """
    Fitness based on the unit score from within the game.
    Player_{unit score} / enemy_{unit_score}
    """
    my_unit_score = int(xml_file.find('player').find('unitScore').get('value')) + \
    int(xml_file.find('player').find('killScore').get('value'))

    enemy_unit_score = int(xml_file.find('enemy').find('unitScore').get('value')) + \
        int(xml_file.find('enemy').find('killScore').get('value'))

    return float(my_unit_score) / enemy_unit_score


def building_score_ratio(xml_file):
    """
    Calculates the ratio of the score component relative to buildings
    :param xml_file:
    :return: building_score of player / building_score of enemy
    """
    my_bldg_score = int(xml_file.find('player').find('buildingScore').get('value')) + \
        int(xml_file.find('player').find('razingScore').get('value'))

    enemy_bldg_score = int(xml_file.find('enemy').find('buildingScore').get('value')) + \
        int(xml_file.find('enemy').find('razingScore').get('value'))
    return float(my_bldg_score) / enemy_bldg_score


def calculate_fitness(f, population, cfg, mode):
    #print f
    path_parts = f.split(os.sep)
    fname = path_parts[-1] #after the last slash we have the file name
    fname_parts = fname.split('.')
    indiv_index = int(fname_parts[0]) #index of the individual is the part in file name before the first dot

    #get xml file path
    xml_path = ""
    for n in range(0, len(path_parts) -1):
        xml_path = xml_path + path_parts[n] + "\\"
    xml_path = xml_path + fname_parts[0] + ".chr.res.xml" #index of the individual is the part in file name before the first dot

    if os.path.exists(xml_path): #check if there is a played game
        #load xml tree
        xml_tree = xml.etree.ElementTree.parse(xml_path).getroot()
        if mode == cfg.SCORE_RATIO:
            fit_value = score_fit(xml_tree)

        elif mode == cfg.UNIT_BASED:
            fit_value = unit_fit(xml_tree)

        elif mode == cfg.TIME_BASED:
            fit_value = time_fit(xml_tree)

        elif mode == cfg.UNIT_SCORE:
            fit_value = unit_score_fit(xml_tree)

        elif mode == cfg.BUILDING_SCORE_RATIO:
            fit_value = building_score_ratio(xml_tree)

        fit_file = open(f, 'w')
        #print fitness
        fit_file.write(str(fit_value))
        fit_file.close()

    else: #get estimated value from .fit file
        fit_value = float(open(f).read().strip())

    population[indiv_index]['fitness'] = fit_value
    #print 'index %d: %.3f' % (indiv_index, population[indiv_index]['fitness'])


def evaluate_victory_ratio(population, generation, cfg):
    """
    Evaluates fitness of population members whose reliability values are below
    the threshold, USING VICTORY RATIO AS FITNESS FUNCTION
    :param population: the array with the population
    :param generation: the number of this generation
    :param cfg: the configparser object
    :return:

    """
    import xml.etree.ElementTree as ET

    # pattern to create .chr file: first %d for individual index;
    # second %d for match number
    chr_file_pattern = '%d-rep-%d'    
    
    sc_dir, cl_path = paths.read_paths()

    # create dir g# in output_path
    write_dir = os.path.join(sc_dir, cfg.output_dir, 'g%d' % generation)
    distutils. dir_util.mkpath(write_dir)

    for index in range(0, len(population)):
        p = population[index]

        '''
        if reliability is above threshold and probability of evaluation in this condition is not met:
        we make random.random() > prob because prob refers to chance of evaluation of reliable individuals
        and this test is for individuals who will NOT be evaluated
        '''
        if p['reliability'] > cfg.reliab_threshold and random.random() > cfg.p_eval_above_thresh:
            # create file with fitness, this individual won't be eval'ed
            # also, we create its chr_file with .lock extension, so that broodwar won't simulate it
            chr_file = open(os.path.join(write_dir, '%d.chr.lock' % index), 'w')
            fit_file = open(os.path.join(write_dir, '%d.fit' % index), 'w')

            chr_file.write(p['chromosome'].to_file_string())
            chr_file.close()

            fit_file.write(str(p['fitness']))
            fit_file.close()
        else:
            # as this individual will be evaluated, we set its reliability to 1 a priori
            p['reliability'] = 1

            # creates multiple files for the same individual, one for each match it will play
            for match in range(cfg.num_matches):
                chr_file = open(os.path.join(write_dir, '%d-rep-%d.chr' % (index, match)), 'w')
                chr_file.write(p['chromosome'].to_file_string())
                chr_file.close()

        # creates a file with the reliability of this individual
        misc_file = open(os.path.join(write_dir, '%d.misc' % index), 'w')
        misc_file.write(str(p['reliability']))
        misc_file.close()

    # creates path.cfg on SC directory in order to orientate c++ where to find the chromosome files
    pfile = open(os.path.join(sc_dir, 'path.cfg'), 'w')
    pfile.write(write_dir)
    pfile.close()

    # calls chaoslauncher, which will run the game for each missing .fit file in the last generation it finds
    print 'Starting simulations...'

    # calls chaoslauncher if not all result files were generated #TODO: make this prettier
    result_files_pattern = os.path.join(write_dir, "*.res.xml")
    result_files = glob.glob(result_files_pattern)

    cl_called = False

    if len(result_files) / cfg.num_matches < cfg.popsize:
        chaosLauncher = subprocess.Popen([cl_path])
        cl_called = True
        time.sleep(5)  # waits for starcraft launch

    # watch directory to see if all .res.xml files were generated
    while True:
        #result_files_pattern = os.path.join(write_dir, "*.fit")
        result_files = glob.glob(result_files_pattern)
        sys.stdout.write(
            '\r %5d .res.xml files found. Need %5d to finish this generation. ' %
            (len(result_files), cfg.popsize*cfg.num_matches)
        )

        if len(result_files) / cfg.num_matches >= cfg.popsize:
            print 'Generation %d finished.' % generation
            break
        #print "%d files with pattern %s" % (len(fit_files), fit_files_pattern)
        time.sleep(1)
        if not rwe.monitor_once(): #problem... relaunch starcraft
            print 'Relaunching Chaoslauncher and StarCraft.'
            chaosLauncher = subprocess.Popen([cl_path])
            time.sleep(2)

    # finishes this execution of chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    if cl_called:
        chaosLauncher.terminate()

    print 'Simulations finished. Collecting fitness information'
    time.sleep(5)

    '''
    For all individuals, read all result files and calculates
    its victory ratio in a number of matches.
    '''
    for index, individual in enumerate(population):
        if individual['reliability'] < 1: # assuming that individuals w/ reliab < 1 were NOT EVALUATED
            continue
        else:  # read all files and calculate victory ratio
            files_pattern = os.path.join(write_dir, "%d-rep-*.res.xml" % index)
            files = glob.glob(files_pattern)
            victories = 0
            for f in files:
                xml_tree = ET.parse(f).getroot()
                if xml_tree.find('result').get('value') == 'win':
                    victories += 1

            individual['fitness'] = float(victories) / cfg.num_matches

            # writes the fitness in a .fit file to orientate run_best_fitness
            fit_file_name = os.path.join(write_dir, '%d.fit' % index)  # name: <index>.fit
            fit_file = open(fit_file_name, 'w')
            fit_file.write(str(individual['fitness']))
            fit_file.close()


def evaluate(population, generation, cfg):
    """
    Evaluates fitness of population members whose reliability values are below
    the threshold
    :param population: the array with the population
    :param generation: the number of this generation
    :param cfg: the configparser object
    :return:

    """

    sc_dir, cl_path = paths.read_paths()

    # create dir g# in output_path
    write_dir = os.path.join(sc_dir, cfg.output_dir, 'g%d' % generation)
    distutils. dir_util.mkpath(write_dir)

    for i in range(0, len(population)):
        p = population[i]

        #if reliability is above threshold and probability of evaluation in this condition is not met:
        #we make random.random() > prob because prob refers to chance of evaluation of reliable individuals
        #and this test is for individuals who will NOT be evaluated

        lock_i = os.path.exists(os.path.join(write_dir, '%d.chr.lock' % i))
        lock_isuc = os.path.exists(os.path.join(write_dir, '%d.chr.lock' % (i+1)))

        
        if (not lock_i or (lock_i and not lock_isuc)):
            if (lock_i):
                os.remove(os.path.join(write_dir, '%d.chr.lock' % i))
            if p['reliability'] > cfg.reliab_threshold and random.random() > cfg.p_eval_above_thresh:
                #create file with fitness, this individual won't be eval'ed
                #also, we create its chr_file with .lock extension, so that broodwar won't simulate it
                chr_file = open(os.path.join(write_dir, '%d.chr.lock' % i), 'w')
                fit_file = open(os.path.join(write_dir, '%d.fit' % i), 'w')
                #print type(p['fitness'])
                fit_file.write(str(p['fitness']))
                fit_file.close()
            else:
                #as this individual will be evaluated, we set its reliability to 1 a priori
                chr_file = open(os.path.join(write_dir, '%d.chr' % i), 'w')
                p['reliability'] = 1

            chr_file.write(p['chromosome'].to_file_string())
            chr_file.close()

            #creates a file with the reliability of this individual
            misc_file = open(os.path.join(write_dir, '%d.misc' % i), 'w')
            misc_file.write(str(p['reliability']))
            misc_file.close()

    #creates path.cfg on SC directory in order to orientate c++ where to find the chromosome files
    pfile = open(os.path.join(sc_dir, 'path.cfg'), 'w')
    pfile.write(write_dir)
    pfile.close()

    #calls chaoslauncher, which will run the game for each missing .fit file in the last generation it finds
    print 'Starting simulations...'

    #calls chaoslauncher if not all fitness files were generated #TODO: make this prettier
    fit_files_pattern = os.path.join(write_dir, "*.fit")
    fit_files = glob.glob(fit_files_pattern)

    cl_called = False

    if len(fit_files) < cfg.popsize:
        chaosLauncher = subprocess.Popen([cl_path])
        cl_called = True

        time.sleep(5)   #waits until StarCraft is launched

    #watch directory to see if all .fit files were generated
    while True:
        fit_files_pattern = os.path.join(write_dir, "*.fit")
        fit_files = glob.glob(fit_files_pattern)
        sys.stdout.write('\r %5d .fit files found. Need %5d to finish this generation. ' % (len(fit_files), cfg.popsize))

        if len(fit_files) / cfg.num_matches >= cfg.popsize:
            print 'Generation %d finished.' % generation
            break
        #print "%d files with pattern %s" % (len(fit_files), fit_files_pattern)
        time.sleep(1)
        if not rwe.monitor_once(): #problem... relaunch starcraft
            print 'Relaunching Chaoslauncher and StarCraft.'
            chaosLauncher = subprocess.Popen([cl_path])
            time.sleep(2)


    #finishes this execution of chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    if(cl_called):
        chaosLauncher.terminate()

    print 'Simulations finished. Collecting fitness information'
    time.sleep(5)

    for f in fit_files:
        calculate_fitness(f, population, cfg, cfg.function)
