import random
import copy
from chromosome import Chromosome

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

def elite(population):
    '''
    Returns the best individual from the population
    :param population:
    :return:
    '''
    return max(population, key=lambda x: x['fitness'])

def crossover(parent1, parent2, p_crossover):
    '''
    Performs crossover with the parents to produce the offspring
    :param parent1:
    :param parent2:
    :param p_crossover:
    :param p_mutation:
    :return: tuple (child1, child2)

    '''
    #par1_chromo = parent1['chromosome']
    #par2_chromo = parent2['chromosome']
    #child1_chromo = copy.deepcopy(par1_chromo)
    #child2_chromo = copy.deepcopy(par2_chromo)

    #gets the arrays with values to perform the exchange
    p1_array = parent1['chromosome'].to_array()
    p2_array = parent2['chromosome'].to_array()

    #initially, children are copies of parents
    c1_array = copy.copy(p1_array)
    c2_array = copy.copy(p2_array)

    assert len(c1_array) == len(c2_array)

    length = len(c1_array)

    if random.random() < p_crossover:
        #select crossover point
        xover_point = random.randint(1, length - 1)
        #print type(par1_chromo._genes), type(child1_chromo._genes)

        #performs one-point exchange around the xover point
        c1_array[0: xover_point] = p1_array[0: xover_point]
        c1_array[xover_point: length] = p2_array[xover_point: length]

        c2_array[0: xover_point] = p2_array[0: xover_point]
        c2_array[xover_point: length] = p1_array[xover_point: length]

    #builds new chromosomes from the (possibly crossovered) arrays
    child1_chromo = Chromosome.from_array(c1_array)
    child2_chromo = Chromosome.from_array(c2_array)

    return (
        {'chromosome': child1_chromo, 'fitness': 0, 'reliability': 0},
        {'chromosome': child2_chromo, 'fitness': 0, 'reliability': 0}
    )

def mutation(individual, p_mutation):
    '''
    Performs mutation with an individual (genes are changed in-place
    :param individual:
    :param p_mutation:
    :return:

    '''

    for name, gene in individual['chromosome']._genes.iteritems():
        if random.random() < p_mutation:
            gene.randomize()