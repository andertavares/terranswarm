import os
import re
import random
import copy
import geneticoperators as genops
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

    return (r1 * f1 * s1 + r2 * f2 * s2) / (r1 * s1 + r2 * s2)


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
    return ((s1*r1)**2 + (s2*r2)**2) / (s1*r1 + s2*r2)


def similarity(child, parent):
    '''
    Returns the similarity between a child and a parent
    :param child:
    :param parent:
    :return: the similarity value in [0..1]

    '''
    chr_length = len(child['chromosome'])

    parent_array = parent.to_array()
    child_array = child.to_array()

    partial = 0.0
    for i in range(0, chr_length):
        partial += abs(parent_array[i].value - child_array[i].value) / \
                   float(child_array[i].domain.max_value - child_array[i].domain.min_value)

    return 1 - partial / chr_length


def start(cfg_file):
    cfg = configparser.ConfigParser(cfg_file)

    #generates initial population
    old_pop = []
    for p in range(0, cfg.popsize):
        #sets fitness to 1 because population will be evaluated right after this loop
        old_pop.append({'chromosome': Chromosome(), 'fitness': 1, 'reliability': 0})

    #evaluates the 1st generation
    evaluate(old_pop, 1, cfg)

    for i in range(1, cfg.generations):  #starts from 1 because 1st generation (index 0) was evaluated already
        new_pop = []

        while len(new_pop) < cfg.popsize:
            (p1, p2) = genops.tournament_selection(old_pop, cfg.tournament_size)
            (c1, c2) = genops.crossover(p1, p2, cfg.p_crossover)
            genops.mutation(c1, cfg.p_mutation)
            genops.mutation(c2, cfg.p_mutation)

            #estimates fitness of children
            c1['fitness'] = estimate_fitness(c1, p1, p2)
            c2['fitness'] = estimate_fitness(c2, p1, p2)

            #calculates reliability of children

        #create [1..n].ch
        #estimate fitness; create i.fit
        #if rel < thresh: evaluate

        #operators


def tournament_selection(population, tournament_size):
    '''
    Executes two tournaments to return the two parents
    :param population: array of individuals
    :param tournament_size: number of contestants in the tournament
    :return: a tuple (parent1, parent2)

    '''
    parents = []

    while len(parents) < 2:  #we need 2 parents

        #selects the contestants of the tournament randomly from the population
        tournament = []
        while len(tournament) < tournament_size:
            tournament.append(random.choice(population))

        #tournament winner is the one with maximum fitness among the contestants
        parents.append(max(tournament, key=lambda x: x['fitness']))

    return tuple(parents)


def crossover_and_mutation(parent1, parent2, p_crossover, p_mutation):
    '''
    Performs crossover and mutation with the parents to produce the offspring
    :param parent1:
    :param parent2:
    :param p_crossover:
    :param p_mutation:
    :return:

    '''


    child1_chromo = copy.copy(parent1['chromosome'])
    child2_chromo = copy.copy(parent2['chromosome'])

    assert len(child1_chromo) == len(child2_chromo)

    length = len(child1_chromo)

    if random.random() < p_crossover:
        #select crossover point
        xover_point = random.randint(1, len(parent1['chromosome'] - 1))

        #performs one-point exchange around the xover point
        child1_chromo[0: xover_point] = parent1[0: xover_point]
        child1_chromo[xover_point: length] = parent2[xover_point: length]

        child2_chromo[0: xover_point] = parent2[0: xover_point]
        child2_chromo[xover_point: length] = parent1[xover_point: length]

    for gene in child1_chromo:
        if random.random < p_mutation:
            gene.randomize()

    for gene in child2_chromo:
        if random.random < p_mutation:
            gene.randomize()

    return (
        {'chromosome': child1_chromo, 'fitness': 0, 'reliability': 0},
        {'chromosome': child2_chromo, 'fitness': 0, 'reliability': 0}
    )


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
    os.mkdir(write_dir)


    for i in range(0, len(population)):
        p = population[i]

        #creates a file and writes the chromosome
        chr_file = open(os.path.join(write_dir, '%d.chr' % i), 'w')
        chr_file.write(p.to_file_string())
        chr_file.close()


        if p['reliability'] >= cfg.reliab_threshold:
            #create file with fitness, this individual won't be eval'ed
            fit_file = open(os.path.join(write_dir, '%d.fit' % i), 'w')
            fit_file.write(p['fitness'])
            fit_file.close()

    #calls chaoslauncher, which will run the game for each missing .fit file in the last generation it finds
    chaosLauncher = subprocess.Popen([cl_path])

    #watch directory to see if all .fit files were generated
    while True:
        fit_files = glob.glob(os.path.join(cfg.output_dir, "*.fit"))
        if len(fit_files) >= cfg.popsize:
            break
        time.sleep(1)

    #finishes this execution of chaoslauncher and starcraft
    chaosLauncher.terminate()
    subprocess.call("taskkill /IM starcraft.exe")

def read_paths():
    #read from paths.ini
    paths = open('paths.ini', 'r').read()

    sc_match_obj = re.match(r'starcraft_dir(.?)=(.?)".*"$', paths, re.M | re.I)
    sc_dir = sc_match_obj.group(1)

    cl_match_obj = re.match(r'chaoslauncher_path(.?)=(.?)".*"$', paths, re.M | re.I)
    cl_path = cl_match_obj.group(1)

    if not os.path.exists(sc_dir):
        raise RuntimeError('Directory to Starcraft was not found. paths.ini says: %s' % sc_dir)

    if not os.path.exists(cl_path):
        raise RuntimeError('Chaoslauncher executable was not found. paths.ini says: %s' % cl_path)

    return (sc_dir, cl_path)