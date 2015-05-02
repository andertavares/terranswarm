'''
Created on Feb 1, 2013

'''

import os
import xml.etree.ElementTree as ET


def str_to_bool(value):
    if value in ['True', 'true']:
        return True
    return False


class ConfigParser(object):
    """
    Parses the .xml file with the configs
    
    """

    #constants for the types of fitness functions
    SCORE_RATIO = 'score_ratio'
    TIME_BASED = 'time_based'
    UNIT_BASED = 'unit_based'
    VICTORY_RATIO = 'victory_ratio'
    UNIT_SCORE = 'unit_score'

    def __init__(self, cfgpath):
        """
        Constructor

        """
        self._set_defaults()

        #list of possible types of fitness functions
        self.possible_fitness = [self.SCORE_RATIO, self.TIME_BASED,
                                 self.UNIT_BASED, self.VICTORY_RATIO,
								 self.UNIT_SCORE]
        
        self.cfgdir = os.path.dirname(os.path.realpath(cfgpath))
        cfgtree = ET.parse(cfgpath)
        
        for io_element in cfgtree.find('io'):

            if io_element.tag == 'output-dir':
                self.output_dir = io_element.get('value')

        if cfgtree.find('fitness') is not None: #prevents error with old configs without <fitness> tag
            for fitness_element in cfgtree.find('fitness'):
                if fitness_element.tag == 'function':
                    self.function = fitness_element.get('value').lower()
                    if self.function not in self.possible_fitness:
                        raise ValueError('Fitness function %s invalid' % self.function)

            #number of matches in victory_ratio function
            if fitness_element.tag == 'num_matches':
                self.num_matches = int(fitness_element.get('value'))

        for param_element in cfgtree.find('parameters'):
            if param_element.tag == 'enemy':
                self.enemy = param_element.get('value').lower() #transforms to lowercase

            if param_element.tag == 'p-crossover':
                self.p_crossover = float(param_element.get('value'))
            
            if param_element.tag == 'p-mutation':
                self.p_mutation = float(param_element.get('value'))
            
            if param_element.tag == 'generations':
                self.generations = int(param_element.get('value'))
            
            if param_element.tag == 'popsize':
                self.popsize = int(param_element.get('value'))
            
            if param_element.tag == 'elitism':
                self.elitism = str_to_bool(param_element.get('value'))
            
            if param_element.tag == 'tournament-size':
                self.tournament_size = int(param_element.get('value'))

            if param_element.tag == 'reliab-threshold':
                self.reliab_threshold = float(param_element.get('value'))

            if param_element.tag == 'p-eval-above-thresh':
                self.p_eval_above_thresh = float(param_element.get('value'))
            
        for ext_element in cfgtree.find('externals'):
            if ext_element.tag == 'random-seed':
                self.random_seed = float(ext_element.get('value'))
            
            if ext_element.tag == 'repetitions':
                self.repetitions = int(ext_element.get('value'))

    def _parse_path(self, value):
        return os.path.join(
            self.cfgdir, os.path.expanduser(value)
        )
        
    def _set_defaults(self):
        self.output_dir = 'testrun'

        self.enemy = None
        self.p_crossover = .6
        self.p_mutation = .001
        self.generations = 30
        self.popsize = 30
        self.elitism = True
        self.tournament_size = 2
        self.reliab_threshold = 0.6
        self.p_eval_above_thresh = 0.05

        self.function = 'score_ratio'
        self.num_matches = 1
        
        self.random_seed = 1
        self.repetitions = 1
