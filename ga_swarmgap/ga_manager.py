import configparser
import os
from chromosome import Chromosome

STARCRAFT_PATH = 'C:/Program Files/Starcraft_old/'

def start(cfg_file):
    cfg = configparser.ConfigParser(cfg_file)

    #generates initial population
    old_pop = []
    for p in range(0, cfg.popsize):
        chromo = Chromosome()
        old_pop[p] = {'chromosome': Chromosome(), 'fitness': 0, 'reliability': 0}


    for i in range(0, cfg.generations):
        new_pop = None
        #create [1..n].ch
        #estimate fitness; create i.fit
        #if rel < thresh: evaluate

        #operators

def evaluate(population, generation):
    '''
    Evaluates fitness of population members whose reliability values are below
    the threshold
    :param population: the array with the population
    :param generation: the number of this generation
    :return:

    '''

    #create dir g# in output_path
    os.mkdir(os.path.join(STARCRAFT_PATH, cfg.output_dir, 'g%d' % generation))

    for p in population:
        if p['reliability'] < cfg.reliab_threshold:
            pass #TODO: resume from here
