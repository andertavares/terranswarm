__author__ = 'Anderson'

import unittest


'''
Created on Sep 22, 2013

@author: anderson
'''
import unittest
import configparser

class Test(unittest.TestCase):

    def test_defaults(self):
        config = configparser.ConfigParser('test/empty.xml')

        self.assertEqual('testrun', config.output_dir)

        self.assertEqual(.6, config.p_crossover)
        self.assertEqual(.001, config.p_mutation)
        self.assertEqual(30, config.generations)
        self.assertEqual(30, config.popsize)
        self.assertEqual(True, config.elitism)
        self.assertEqual(2, config.tournament_size)
        self.assertEqual(.6, config.reliab_threshold)

        self.assertEqual(1, config.random_seed)
        self.assertEqual(1, config.repetitions)

    def test_parsing(self):
        #assumes _parse_path is correct
        config = configparser.ConfigParser('runs/test/config.xml')

        self.assertEqual('test', config.output_dir)

        self.assertEqual(.9, config.p_crossover)
        self.assertEqual(.01, config.p_mutation)
        self.assertEqual(50, config.generations)
        self.assertEqual(30, config.popsize)
        self.assertEqual(True, config.elitism)
        self.assertEqual(5, config.tournament_size)
        self.assertEqual(.6, config.reliab_threshold)

        self.assertEqual(6, config.random_seed)
        self.assertEqual(1, config.repetitions)



if __name__ == '__main__':
    unittest.main()
