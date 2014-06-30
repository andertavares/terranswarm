import os
import re
import shutil
import distutils.dir_util
import random
import copy
import geneticoperators as genops
import chromosome as chromo
import subprocess
import time
import glob

import configparser
from chromosome import Chromosome

#CHAOSLAUNCHER = "C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher.exe"
#STARCRAFT_PATH = 'C:/Program Files/Starcraft_old/'


def estimate_fitness(c, p1, p2):
    '''
    Estimates fitness of a child (c), based on the parents (p1, p2) data
    :param c: the child
    :param p1: a parent
    :param p2: the other parent
    :return: the estimated fitness

    '''

    #creates aliases for the variables that enter the equation
    s1, s2 = similarity(p1, c), similarity(p2, c)
    r1, r2 = p1['reliability'], p2['reliability']
    f1, f2 = p1['fitness'], p2['fitness']

    return ((r1 * f1 * s1) + (r2 * f2 * s2)) / float((r1 * s1) + (r2 * s2))


def reliability(c, p1, p2):
    '''
    Returns the reliability of a child given its parents
    :param c: the child
    :param p1: a parent
    :param p2: the other parent
    :return: the calculated reliability

    '''

    #calculates the similarities between the child and the two parents
    s1, s2 = similarity(p1, c), similarity(p2, c)
    r1, r2 = p1['reliability'], p2['reliability']

    #uses reliability equation to calculate reliability
    return ((s1*r1)**2 + (s2*r2)**2) / float(s1*r1 + s2*r2)


def similarity(child, parent):
    '''
    Returns the similarity between a child and a parent
    :param child:
    :param parent:
    :return: the similarity value in [0..1]

    '''
    chr_length = child['chromosome'].size

    parent_array = parent['chromosome']._genes
    child_array = child['chromosome']._genes

    partial = 0.0
    for i in range(0, chr_length):
        index = child['chromosome'].GENE_NAMES[i]
        partial += abs(parent_array[index].value - child_array[index].value) / \
                   abs(float(child_array[index].domain.max_value() - child_array[index].domain.min_value()))

    return 1 - (partial / chr_length)


def start(cfg_file):
    cfg = configparser.ConfigParser(cfg_file)
    random.seed(cfg.random_seed)

    #copies bwapi.ini from experiment dir to starcraft dir
    try:
        sc_dir, cl_path = read_paths()
        exp_inipath = os.path.join(os.path.dirname(os.path.abspath(cfg_file)), 'bwapi.ini')
        sc_inipath = os.path.join(sc_dir, 'bwapi-data', 'bwapi.ini')
        shutil.copyfile(exp_inipath, sc_inipath)
    except IOError:
        print 'An error has occurred. Probably you don\'t have a bwapi.ini\n' \
              'file in the same dir as the configuration .xml file.\n' \
              'Please create a bwapi.ini file in \n' \
              '%s \n' \
              'with the configurations for starcraft execution (enemy_race, etc).' % exp_inipath

        exit()

    #generates initial population
    old_pop = []
    for p in range(0, cfg.popsize):
        #sets fitness to 1 because population will be evaluated right after this loop
        old_pop.append({'chromosome': Chromosome(), 'fitness': 1, 'reliability': 0})

    #evaluates the 1st generation
    print 'Evaluating generation #1'
    evaluate(old_pop, 1, cfg)

    for i in range(1, cfg.generations):  #starts from 1 because 1st generation (index 0) was evaluated already
        new_pop = []

        if cfg.elitism:
            #adds the best individual from previous population
            new_pop.append(genops.elite(old_pop))

        while len(new_pop) < cfg.popsize:
            (p1, p2) = genops.tournament_selection(old_pop, cfg.tournament_size)
            (c1, c2) = genops.crossover(p1, p2, cfg.p_crossover)
            genops.mutation(c1, cfg.p_mutation)
            genops.mutation(c2, cfg.p_mutation)

            #estimates fitness of children
            c1['fitness'] = estimate_fitness(c1, p1, p2)
            c2['fitness'] = estimate_fitness(c2, p1, p2)

            #calculates reliability of children
            c1['reliability'] = reliability(c1, p1, p2)
            c2['reliability'] = reliability(c2, p1, p2)

            #adds children to the new population
            #because of elitism, we test if second child can be added
            #otherwise, new population size can be 1 + old pop. size
            new_pop.append(c1)
            if (len(new_pop) < cfg.popsize):
                new_pop.append(c2)


        #new population built, now evaluates it. Generation number is i+1
        print 'Evaluating generation #%d' % (i+1)
        evaluate(new_pop, i+1, cfg)

        #prepares for the next generation
        old_pop = new_pop


def evaluate(population, generation, cfg):
    '''
    Evaluates fitness of population members whose reliability values are below
    the threshold
    :param population: the array with the population
    :param generation: the number of this generation
    :param cfg: the configparser object
    :return:

    '''

    sc_dir, cl_path = read_paths()

    #create dir g# in output_path
    write_dir = os.path.join(sc_dir, cfg.output_dir, 'g%d' % generation)
    distutils. dir_util.mkpath(write_dir)


    for i in range(0, len(population)):
        p = population[i]

        #if reliability is above threshold and probability of evaluation in this condition is not met:
        #we make random.random() > prob because prob refers to chance of evaluation of reliable individuals
        #and this test is for individuals who will NOT be evaluated
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
    print 'starting simulations...'

    #calls chaoslauncher if not all fitness files were generated #TODO: make this prettier
    fit_files_pattern = os.path.join(write_dir, "*.fit")
    fit_files = glob.glob(fit_files_pattern)

    cl_called = False

    if len(fit_files) < cfg.popsize:
        chaosLauncher = subprocess.Popen([cl_path])
        cl_called = True

    #watch directory to see if all .fit files were generated
    while True:
        fit_files_pattern = os.path.join(write_dir, "*.fit")
        fit_files = glob.glob(fit_files_pattern)
        if len(fit_files) >= cfg.popsize:
            break
        #print "%d files with pattern %s" % (len(fit_files), fit_files_pattern)
        time.sleep(1)

    #finishes this execution of chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    if(cl_called):
        chaosLauncher.terminate()

    print 'Simulations finished. Collecting fitness information'
    time.sleep(5)

    for f in fit_files:
        print f
        path_parts = f.split(os.sep)
        fname = path_parts[-1] #after the last slash we have the file name
        fname_parts = fname.split('.')
        indiv_index = int(fname_parts[0]) #index of the individual is the part in file name before the first dot


        fit_value = float(open(f).read().strip())
        #print 'Fitness for individual %d= %d ' % (indiv_index, fit_value)
        population[indiv_index]['fitness'] = fit_value


def read_paths():
    #read from paths.ini
    paths = open('paths.ini', 'r').read()

    sc_dir = re.match(r'.*starcraft_dir.?=.?"(.*?)"', paths, re.M | re.I | re.DOTALL).group(1)
    #sc_dir = sc_match_obj.group(1)

    cl_path = re.match(r'.*chaoslauncher_path.?=.?"(.*?)"', paths, re.M | re.I | re.DOTALL).group(1)

    if not os.path.exists(sc_dir):
        raise RuntimeError('Directory to Starcraft was not found. paths.ini says: %s' % sc_dir)

    if not os.path.exists(cl_path):
        raise RuntimeError('Chaoslauncher executable was not found. paths.ini says: %s' % cl_path)

    return (sc_dir, cl_path)