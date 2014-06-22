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
    '''
    Parses the .xml file with the configs
    
    '''
    def __init__(self, cfgpath):
        '''
        Constructor

        '''
        self._set_defaults()
        
        self.cfgdir = os.path.dirname(os.path.realpath(cfgpath))
        cfgtree = ET.parse(cfgpath)
        
        
        #print 'Config file parsing started...'

        for io_element in cfgtree.find('io'):

            if io_element.tag == 'output-dir':
                self.output_dir = io_element.get('value')

        for param_element in cfgtree.find('parameters'):
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

        self.p_crossover = .6
        self.p_mutation = .001
        self.generations = 30
        self.popsize = 30
        self.elitism = True
        self.tournament_size = 2
        self.reliab_threshold = 0.6
        self.p_eval_above_thresh = 0.05
        
        self.random_seed = 1
        self.repetitions = 1
