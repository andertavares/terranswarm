import os
import subprocess
import time
import glob

import configparser
from chromosome import Chromosome

CHAOSLAUNCHER = "C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher.exe"
STARCRAFT_PATH = 'C:/Program Files/Starcraft_old/'


def estimate_fitness(c, p1, p2):
    '''
    Estimates fitness of a child (c), based on the parents (p1, p2) data
    :param c: the child
    :param p1: a parent
    :param p2: the other parent
    :return: the estimated fitness

    '''
    return (
       p1['reliability'] * p1['fitness'] * similarity(p1, c) +
       p2['reliability'] * p2['fitness'] * similarity(p2, c)
    ) / (p1['reliability'] * similarity(p1, c) + p2['reliability'] * similarity(p2, c))

def similarity(child, parent):
    '''
    Returns the similarity between a child and a parent
    :param child:
    :param parent:
    :return: the similarity value in [0..1]

    '''
    chr_length = len(child['chromosome'])

    #TODO: resume from here


def start(cfg_file):
    cfg = configparser.ConfigParser(cfg_file)

    #generates initial population
    old_pop = []
    for p in range(0, cfg.popsize):
        #sets fitness to 1 because population will be evaluated right after this loop
        old_pop[p] = {'chromosome': Chromosome(), 'fitness': 1, 'reliability': 0}

    #evaluates the 1st generation
    evaluate(old_pop, 1, cfg)

    for i in range(1, cfg.generations): #starts from 1 because 1st generation (index 0) was evaluated already
        new_pop = []

        while len(new_pop) < cfg.popsize:
            (p1, p2) = tournament_selection(old_pop)
            (c1, c2) = crossover_and_mutation(p1, p2)

            #estimates fitness of children
            c1['fitness'] = estimate_fitness(c1, p1, p2)

        #create [1..n].ch
        #estimate fitness; create i.fit
        #if rel < thresh: evaluate

        #operators

def evaluate(population, generation, cfg):
    '''
    Evaluates fitness of population members whose reliability values are below
    the threshold
    :param population: the array with the population
    :param generation: the number of this generation
    :param cfg: the configparser object
    :return:

    '''

    #create dir g# in output_path
    write_dir = os.path.join(STARCRAFT_PATH, cfg.output_dir, 'g%d' % generation)
    os.mkdir(write_dir)


    for i in range(0,len(population)):
        p = population[i]

        #creates a file and writes the chromosome
        chr_file = open(os.path.join(write_dir,'%d.chr' % i), 'w')
        chr_file.write(','.join(p.to_array()))
        chr_file.close()
        if p['reliability'] >= cfg.reliab_threshold:
            #create file with fitness, this individual won't be eval'ed
            fit_file = open(os.path.join(write_dir,'%d.fit' % i), 'w')
            fit_file.write(p['fitness'])
            fit_file.close()

    #calls chaoslauncher, which will run the game for each missing .fit file in the last generation it finds
    chaosLauncher = subprocess.Popen([CHAOSLAUNCHER])

    #watch directory to see if all .fit files were generated
    while(True):
        fit_files = glob.glob(os.path.join(cfg.output_dir,"*.fit"))
        if len(fit_files) >= cfg.popsize:
            break
        time.sleep(1)

    #finishes this execution of chaoslauncher and starcraft
    chaosLauncher.terminate()
    subprocess.call("taskkill /IM starcraft.exe")
