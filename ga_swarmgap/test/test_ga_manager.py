import unittest
import domain
from chromosome import Gene
import ga_manager as gam

class TestGaManager(unittest.TestCase):
    def test_estimate_fitness(self):
        pass

    def test_similarity(self):
        #similarity formula: #1 - (sum( abs(ci - pi))/ (abs(maxi - mini)) / length

        #an easy one:
        parent1 = {'chromosome': SimpleChromosome([2, .3])}
        child1 = {'chromosome': SimpleChromosome([2, .3])}
        self.assertEqual(1, gam.similarity(child1, parent1))

        #a not so easy one:
        parent2 = {'chromosome': SimpleChromosome([4, .5])}
        child2 = {'chromosome': SimpleChromosome([2, .3])}

        #hand calculations for the given chromosomes:
        #1 - ((4-2) / (5-0) + (.5-.3) / (.5-.1)) / 2 = .55
        self.assertEqual(.55, gam.similarity(child2, parent2))

        #another one (parent is the same as above - parent2):
        child3 = {'chromosome': SimpleChromosome([3, .4])}

        #hand calculations for the given chromosomes:
        #1 - ((4-2) / (5-0) + (.5-.3) / (.5-.1)) / 2 = .55
        self.assertAlmostEqual(.775, gam.similarity(child3, parent2), None, None, 0.0001)

    def test_reliability(self):
        '''
        Assumes that similarity function is correct

        '''
        parent1 = {'chromosome': SimpleChromosome([3, .4]), 'reliability': 1}
        parent2 = {'chromosome': SimpleChromosome([4, .5]), 'reliability': 1}
        child = {'chromosome': SimpleChromosome([2, .3])}

        #s1: similarity between parent1 and child; s2: similarity between parent2 and child
        #r1 and r2 are reliability for parent1 and parent2, respectively
        #s1 and s2 were hand calculated; r1 and r2 were stipulated
        #s1, s2 = .775, .55
        #r1, r2 = 1, 1

        #hand-calculating reliability (.775^2 + .55^2) / (.55 + .775)
        self.assertAlmostEqual(0.6816, gam.reliability(child, parent1, parent2), None, None, 0.0001)

        #testing with r1 = .7 and r2 = .9
        #hand-calculating reliability ((.775*.7)^2) + (.55*.9)^2) / (.55*.9 + .775*.7)
        parent1['reliability'] = .7
        parent2['reliability'] = .9
        self.assertAlmostEqual(.5198, gam.reliability(child, parent1, parent2), None, None, 0.0001)


    def test_estimate_fitness(self):
        '''
        Assumes that similarity function is working

        '''

        #same chromosomes used in test_reliability, with fitness added
        parent1 = {'chromosome': SimpleChromosome([3, .4]), 'reliability': .7, 'fitness': 75000 }
        parent2 = {'chromosome': SimpleChromosome([4, .5]), 'reliability': .9, 'fitness': 25520}
        child = {'chromosome': SimpleChromosome([2, .3])}

        #s1: similarity between parent1 and child; s2: similarity between parent2 and child
        #r1 and r2 are reliability for parent1 and parent2, respectively
        #s1 and s2 were hand calculated; r1 and r2 were stipulated
        #s1, s2 = .775, .55

        #hand-calculating estimated fitness: ((75000 * .775 * .7) + (25520 * .55 * .9)) / ((.775*.7) + (.55*.9))
        self.assertAlmostEqual(51392.6746, gam.estimate_fitness(child, parent1, parent2), None, None, 0.0001)


class SimpleChromosome(object):
    '''
    Class that mocks up Chromosome in order to do the tests
    '''

    int_domain = domain.DiscreteDomain([0,1,2,3,4,5])
    float_domain = domain.DiscreteDomain([.1, .2, .3, .4, .5])
    GENE_NAMES = ['int', 'float']

    def __init__(self, values):
        self._genes = {
            'int': Gene('int', self.int_domain, values[0]),
            'float': Gene('float', self.float_domain, values[1])
        }

    @property
    def size(self):
        return len(self._genes)


if __name__ == '__main__':
    unittest.main()
