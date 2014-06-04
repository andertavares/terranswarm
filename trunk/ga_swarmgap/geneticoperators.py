import random
import copy

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

def crossover(parent1, parent2, p_crossover):
    '''
    Performs crossover with the parents to produce the offspring
    :param parent1:
    :param parent2:
    :param p_crossover:
    :param p_mutation:
    :return: tuple (child1, child2)

    '''
    par1_chromo = parent1['chromosome']
    par2_chromo = parent2['chromosome']
    child1_chromo = copy.copy(par1_chromo)
    child2_chromo = copy.copy(par2_chromo)

    assert len(child1_chromo) == len(child2_chromo)

    length = len(child1_chromo)

    if random.random() < p_crossover:
        #select crossover point
        xover_point = random.randint(1, len(parent1['chromosome']) - 1)

        #performs one-point exchange around the xover point
        child1_chromo[0: xover_point] = par1_chromo[0: xover_point]
        child1_chromo[xover_point: length] = par2_chromo[xover_point: length]

        child2_chromo[0: xover_point] = par2_chromo[0: xover_point]
        child2_chromo[xover_point: length] = par1_chromo[xover_point: length]

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

    for gene in individual['chromosome']:
        if random.random() < p_mutation:
            gene.randomize()