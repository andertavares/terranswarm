import os
import xml.etree.ElementTree as ET
import paths
import sys
import shutil
import glob
import argparse
import subprocess
import configparser


def validate(results_path, fitness='unit_score'):
    """
    Traverses the generations inside results_path checking whether the fitness in .fit
    file reflects the one configured in fitness_mode
    :param config_file: path to the .xml config file
    :return: int number of invalid fitnessess found
    """

    result_ext = '.res.xml'
    invalids = 0
    for f in glob.glob(os.path.join(results_path,'*','*.fit')):
        #print f
        file_name, file_extension = os.path.splitext(f)
        '''if 'chr' in file_name:
            file_name, chr_ext = os.path.splitext(file_name)'''

        fitness_in_file = float(open(f).read().strip())

        ###works only for unit_score ###
        xml_filename = '%s%s' % (file_name, result_ext)
        xml_tree = ET.parse(xml_filename).getroot()

        my_unit_score = int(xml_tree.find('player').find('unitScore').get('value')) + \
        int(xml_tree.find('player').find('killScore').get('value'))

        enemy_unit_score = int(xml_tree.find('enemy').find('unitScore').get('value')) + \
            int(xml_tree.find('enemy').find('killScore').get('value'))

        fitness_in_results = float(my_unit_score) / enemy_unit_score

        if abs(fitness_in_file - fitness_in_results) > 0.005:  # uses a small tolerance
            #print '.fit says %f and .xml.res says %f for for %s' % (fitness_in_file, fitness_in_results, f)
            invalids += 1

    return invalids






if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Adds the results of an experiment to a .zip file '
                                                 'And commits it to git repository.')

    parser.add_argument('paths', metavar='path', type=str, nargs='*',
                       help='Path to root directory of experiment results')

    parser.add_argument('-f', '--fitness', type=str, default='unit_score',
                       help='Fitness mode to validate')

    args = parser.parse_args()
   
    for path in args.paths:
        if os.path.exists(path):
            invalids = validate(path, args.fitness)
            if invalids > 0:
                print '%s has %d invalid fitness.' % (path, invalids)
            else:
                print '%s ok.' % path
        else:
            print '%s not found. skipping...' % path

    print 'DONE.'
