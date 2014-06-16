import unittest
import geneticoperators
import chromosome as chromo
import domain
import random

class TestGeneticOperators(unittest.TestCase):

    def setUp(self):
        self._mutation_calls = 0
        self._tournament_calls = 0

    def patch_randint(self, min, max):
        assert min == 1
        assert max == 3
        return self._rand_return

    def rand_for_mutation(self):
        self._mutation_calls += 1

        if self._mutation_calls == 2: #ensures that mutation happens at 2nd call
            return 0.0
        return 1.0

    def choice_for_mutation(self, array):
        return 0.999

    def choice_for_tournament(self, array):
        self._tournament_calls += 1

        if self._tournament_calls == 1:
            return {
                'chromosome': [
                    chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.1),
                ],
                'fitness': 50
            }
        elif self._tournament_calls == 2:
            return {
                'chromosome': [
                    chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.2),
                ],
                'fitness': 0
            }
        elif self._tournament_calls == 3:
            return  {
                'chromosome': [
                    chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.3),
                ],
                'fitness': 0
            }
        elif self._tournament_calls == 4:
            return  {
                'chromosome': [
                    chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.4),
                ],
                'fitness': 50
            }
        else:
            raise RuntimeError("Too many tournament calls for random.choice")



    def test_crossover(self):
        p1 = {
            'chromosome': [
                chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.1),
                chromo.Gene('2', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.2),
                chromo.Gene('3', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.3),
                chromo.Gene('4', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.4),
            ]
        }

        p2 = {
            'chromosome': [
                chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.5),
                chromo.Gene('2', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.6),
                chromo.Gene('3', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.7),
                chromo.Gene('4', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.8),
            ]
        }

        #replaces standard library methods...
        self._rand_return = 2
        random.randint = self.patch_randint

        c1, c2 = geneticoperators.crossover(p1, p2, 1.0)

        c1_values = [gene.value for gene in c1['chromosome']]
        c2_values = [gene.value for gene in c2['chromosome']]
        self.assertEqual(c1_values, [0.1, 0.2, 0.7, 0.8])
        self.assertEqual(c2_values, [0.5, 0.6, 0.3, 0.4])


    def test_mutation(self):
        p1 = {
            'chromosome': [
                chromo.Gene('1', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.1),
                chromo.Gene('2', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.2),
                chromo.Gene('3', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.3),
                chromo.Gene('4', domain.DiscreteDomain(domain.STANDARD_INTERVAL), 0.4),
            ]
        }

        #patches methods from random
        random.random = self.rand_for_mutation
        random.choice = self.choice_for_mutation

        geneticoperators.mutation(p1, 0.5)

        p1_values = [gene.value for gene in p1['chromosome']]

        self.assertEqual(p1_values, [0.1, 0.999, 0.3, 0.4])

    def test_tournament_selection(self):
        #patches random.choice for convenient selection of individuals
        #we want individuals 1 and 4 to be the winners, they have values
        # of .1 and .4, respectively
        random.choice = self.choice_for_tournament

        (w1, w2) = geneticoperators.tournament_selection([], 2)
        w1_values = [gene.value for gene in w1['chromosome']]
        w2_values = [gene.value for gene in w2['chromosome']]

        self.assertEqual([0.1], w1_values)
        self.assertEqual([0.4], w2_values)

if __name__ == '__main__':
    unittest.main()
