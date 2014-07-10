import random
import domain


class Gene(object):
    '''
    Encapsulates a chromosome component

    '''

    def __init__(self, name, domain, value=None):
        self._name = name
        self.set_domain(domain)

        if value is not None:
            self._value = value
        else:
            self.randomize()

       # if not self.is_valid():
       #     raise ValueError("Value %d not valid in %s domain" % (self._value, self._name))

    def randomize(self):
        '''
        Assigns itself a random value

        '''
        self._value = self._domain.random_value()

    def set_domain(self, domain):
        '''
        Sets the domain of this gene.

        '''
        self._domain = domain

    def set_value(self, value):
        self._value = value

    @property
    def name(self):
        return self._name

    @property
    def value(self):
        return self._value

    @property
    def domain(self):
        return self._domain

    def is_valid(self):
        '''
        Returns whether the value of this gene belongs to its domain
        '''
        return self._domain.has_value(self._value)


class Chromosome(object):
    '''
    Encapsulates a chromosome for the genetic algorithm
    '''

    '''
    Array with the names of the chromosome components. Prefixes are according
    to the paper nomenclature: s for incentives, k for capabilities. m stands for miscellaneous,
    i.e., non-swarm-gap parameters
    '''
    GENE_NAMES = [
        's_gather_minerals', 's_build_barracks_denominator', 's_build_cmd_center',
        's_attack',
        #'s_attack_near', 's_attack_mid', 's_attack_far',
        's_train_scv_denominator',
        's_train_medic_ratio',
        's_train_marine', 'k_scv_gather_minerals', #'k_scv_build_barracks',
        #'k_scv_build_supply', 'k_scv_build_cmd_center',
        'k_scv_repair_near', 'k_scv_repair_mid', 'k_scv_repair_far',
        'k_scv_explore', 'k_scv_attack_near', 'k_scv_attack_mid', 'k_scv_attack_far',
        'k_marine_explore', 'k_marine_attack_near', 'k_marine_attack_mid', 'k_marine_attack_far',
        'k_general_train_scv', 'k_general_train_marine',
        'k_general_train_medic',
        'm_pack_size'
    ]
    #some tasks or skills are hard-coded and do not enter in the gene: k_build refinery, k_build academy,
    #k_build cmd center, k_build barracks, s/k_build supply depot,
    #research U_238_shells, research stim pack and gather gas.

    def __init__(self, value_array=None):
        '''
        Initializes the genes of this chromosome with the values in the value_array.
        Values must follow the order in the GENE_NAMES array
        :param value_array:
        :return:
        '''

        #creates the chromosome. Genes with specific domains are fixed later
        self._genes = {}
        for i in range(0, len(self.GENE_NAMES)):

            #assigns none if no value array was passed
            value = None
            if value_array is not None:
                value = value_array[i]

            self._genes[self.GENE_NAMES[i]] = Gene(self.GENE_NAMES[i], domain.DiscreteDomain(domain.STANDARD_INTERVAL), value)

        #--- BEGIN: fixes information of genes with specific domains
        self._genes['s_build_barracks_denominator'].set_domain(domain.DiscreteDomain([1, 2, 3, 4, 5]))
        self._genes['s_train_scv_denominator'].set_domain(domain.DiscreteDomain([1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0]))
        self._genes['s_train_medic_ratio'].set_domain(domain.DiscreteDomain([1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0]))
        self._genes['m_pack_size'].set_domain(domain.DiscreteDomain(range(6, 24, 2)))

        if value_array is None:
            self._genes['s_build_barracks_denominator'].randomize()
            self._genes['s_train_scv_denominator'].randomize()
            self._genes['s_train_medic_ratio'].randomize()
            self._genes['m_pack_size'].randomize()
        #--- END: fixes information of genes with specific domains

        #checks if values are valid
        for gene in self._genes.values():
            if not gene.is_valid():
                raise ValueError("Value %d not valid in %s domain" % (gene.value, gene.name))


    @property
    def size(self):
        return len(self._genes)

    @classmethod
    def from_array(cls, arr):
        '''
        Sets the chromosome genes from an array. Elements *must* be in the order given by GENE_NAMES array

        :param arr:
        :return:
        '''

        return cls(arr)

    def to_array(self):
        '''
        Returns the chromosome genes as an array. Gene order obeys the GENE_NAMES array
        :return: array

        '''
        arr = []
        for i in range(0, len(self.GENE_NAMES)):
            arr.append(self._genes[self.GENE_NAMES[i]].value)

        return arr

    def to_file_string(self):
        '''
        Returns a string ready to be read by the c++ swarmgap module. Format is:
        0 value0
        1 value1

        The mapping of indexes to attributes is explicited in GENE_INDEXES variable

        :return: str

        '''

        the_string = ""
        for i in range(len(self.GENE_NAMES)):
            the_string += "%d %s\n" % (i, self._genes[self.GENE_NAMES[i]].value)

        return the_string

    @classmethod
    def from_file(cls, path_to_file):
        '''
        Returns a Chromosome object with the file contents. File format is the same
        used by the C++ module:
        0 value0
        1 value1
        ...
        where 0, 1,..., n are the genes in 0th, 1th,etc. position of GENE_NAMES array

        :return: Chromosome object
        '''
        chr_array = []

        f = open(path_to_file)
        for line in f.readlines():
            index, value = line.split(' ')
            chr_array.append(float(value)) #does not uses index, assuming that file comes with ordered genes

        f.close()

        return Chromosome.from_array(chr_array)
