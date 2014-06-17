import unittest
import chromosome as chromo
import domain
'''
This test modules assumes that domain module has passed its own test

'''

class TestGene(unittest.TestCase):
    def test_random_init(self):
        values = [3, 5, 6, 7]
        g = chromo.Gene('new_name', domain.DiscreteDomain(values))

        self.assertEqual('new_name', g.name)
        self.assertTrue(g.is_valid())
        self.assertTrue(g.value in values)

    def test_defined_init(self):
        values = [3, 5, 6, 7]
        g = chromo.Gene('new_name', domain.DiscreteDomain(values), 5)

        self.assertEqual('new_name', g.name)
        self.assertTrue(g.is_valid())
        self.assertTrue(g.value in values)
        self.assertEquals(5, g.value)

    def test_invalid_init(self):
        values = [3, 5, 6, 7]
        g = chromo.Gene('new_name', domain.DiscreteDomain(values), 4)
        self.assertFalse(g.is_valid())


class TestChromosome(unittest.TestCase):
    def test_random_init(self):
        c = chromo.Chromosome()

        values = []

        for i in range(1,100):
            #tests the domain of the genes
            for g in c._genes.values():
                if g.name == 's_build_barracks_denominator':
                    values = [1, 2, 3, 4, 5]

                elif g.name == 's_train_scv_denominator':
                    values = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0]

                elif g.name == 's_train_medic_ratio':
                    values = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0]

                elif g.name == 'm_pack_size':
                    values = range(6, 24, 2)

                else:
                    values = domain.STANDARD_INTERVAL

                self.assertTrue(g.value in values)

    def test_valid_init(self):
        chromo_values = [
            .8, 3, .15,
            .15, 3,
            3,
            .8, .95,
            .50, .25, .15,
            .1, .95, .3, 0,
            .3, .90, .8, .7,
            .3, .4,
            .5,
            8
        ]
        c = chromo.Chromosome(chromo_values)

        #self.assertEqual(len(chromo_values), len(c._genes))

        for i in range(0, len(chromo.Chromosome.GENE_NAMES)):
            self.assertEqual(
                chromo_values[i], c._genes[chromo.Chromosome.GENE_NAMES[i]].value, chromo.Chromosome.GENE_NAMES[i]
            )

    def test_invalid_init(self):
        chromo_values = [
            .8, 3, .15,
            .15, 3,
            7, #<< this is the invalid value
            .8, .95,
            .50, .25, .15,
            .1, .95, .3, 0,
            .3, .90, .8, .7,
            .3, .4,
            .5,
            8
        ]

        with self.assertRaises(ValueError):
            c = chromo.Chromosome(chromo_values)

        chromo_values = [
            .8, 3, .15,
            .15, 3,
            3,
            .8, .95,
            .50, .25, .15,
            .1, .95, .3, 0,
            .3, .90, .8, .7,
            .3, .4,
            .51, #<< now this is the invalid value
            8
        ]
        with self.assertRaises(ValueError):
            c = chromo.Chromosome(chromo_values)

    def test_to_array(self):
        pass

    def test_to_file_string(self):
        chromo_values = [
            .8, 3, .15,
            .15, 3,
            3,
            .8, .95,
            .50, .25, .15,
            .1, .95, .3, 0,
            .3, .90, .8, .7,
            .3, .4,
            .5,
            8
        ]

        expected_string = '0 0.8\n1 3\n2 0.15\n3 0.15\n4 3\n5 3\n6 0.8\n7 0.95\n8 0.5\n9 0.25\n' \
                          '10 0.15\n11 0.1\n12 0.95\n13 0.3\n14 0\n15 0.3\n16 0.9\n17 0.8\n18 0.7\n19 0.3\n20 0.4' \
                          '\n21 0.5\n22 8\n'

        c = chromo.Chromosome(chromo_values)

        self.assertEqual(expected_string, c.to_file_string())

if __name__ == '__main__':
    unittest.main()
