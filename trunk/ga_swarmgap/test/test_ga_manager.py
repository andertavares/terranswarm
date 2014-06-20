import unittest
import domain
from chromosome import Gene
import ga_manager as gam

class TestGaManager(unittest.TestCase):
    def test_estimate_fitness(self):
        pass

    def test_similarity(self):
        #an easy one:
        parent = {'chromosome': SimpleChromosome([2, .3])}
        child = {'chromosome': SimpleChromosome([2, .3])}
        self.assertEqual(1, gam.similarity(child, parent))

        #a not so easy one:
        parent = {'chromosome': SimpleChromosome([2, .3])}
        child = {'chromosome': SimpleChromosome([4, .5])}

        #1 - (sum( abs(ci - pi))/ (abs(maxi - mini)) / length

        #hand calculations for the given chromosomes:
        #1 - ((4-2) / (5-0) + (.5-.3) / (.5-.1)) / 2 = .55
        self.assertEqual(.55, gam.similarity(child, parent))


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
