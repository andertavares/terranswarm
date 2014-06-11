import random
from abc import ABCMeta, abstractmethod

STANDARD_INTERVAL = [a / 100.0 for a in range(0, 101, 5)] #the interval [0, 0.05, 0.10, ..., 0.95, 1.00]


class AbstractDomain(object):
    '''
    Encapsulates the domain (set of values) a gene can assume.

    '''
    __metaclass__ = ABCMeta


    @abstractmethod
    def has_value(self, value):
        '''
        Returns whether the domain has the given value

        '''
        raise NotImplementedError("Only Domain subclasses should be instantiated.")

    @abstractmethod
    def random_value(self):
        '''
        Returns a random value from the domain

        '''
        raise NotImplementedError("Only Domain subclasses should be instantiated.")

    @abstractmethod
    def max_value(self):
        '''
        Returns the maximum value of this domain

        '''
        raise NotImplementedError("Only Domain subclasses should be instantiated.")

    @abstractmethod
    def min_value(self):
        '''
        Returns the minimum value of this domain

        '''
        raise NotImplementedError("Only Domain subclasses should be instantiated.")


class DiscreteDomain(AbstractDomain):
    def __init__(self, possible_values):
        self._possible_values = possible_values

    def has_value(self, value):
        return value in self._possible_values

    def random_value(self):
        return random.choice(self._possible_values)

    def max_value(self):
        return max(self._possible_values)

    def min_value(self):
        return min(self._possible_values)