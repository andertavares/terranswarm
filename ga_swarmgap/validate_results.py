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

    if fitness == 'unit_score':
        return validate_unit_score(results_path)

    elif fitness == 'time_based':
        return validate_time_based(results_path)

    elif fitness == 'score_ratio':
        return validate_score_ratio(results_path)

    elif fitness == 'victory_ratio':
        return validate_victory_ratio(results_path)

    else:
        print 'INVALID FITNESS MODE.'
        return -1


def validate_time_based(results_path):
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

        fitness_in_file = float(open(f).read().strip())

        xml_filename = '%s%s' % (file_name, result_ext)
        xml_tree = ET.parse(xml_filename).getroot()

        result = xml_tree.find('result').get('value')
        duration = int(xml_tree.find('gameDuration').get('value'))

        fitness_in_results = 0.0
        if result == 'win':
            fitness_in_results = 1.0 - (duration / 7200.0)
        else:
            fitness_in_results = duration / 7200.0

        if abs(fitness_in_file - fitness_in_results) > 0.005:  # uses a small tolerance
            #print '.fit says %f and .xml.res says %f for for %s' % (fitness_in_file, fitness_in_results, f)
            invalids += 1

    return invalids


def validate_unit_score(results_path):
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


def validate_score_ratio(results_path):
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

        fitness_in_results = float(xml_tree.find('scoreRatio').get('value'))

        if abs(fitness_in_file - fitness_in_results) > 0.005:  # uses a small tolerance
            #print '.fit says %f and .xml.res says %f for for %s' % (fitness_in_file, fitness_in_results, f)
            invalids += 1

    return invalids


def validate_victory_ratio(results_path):
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
        if '-rep-' in f:  #skips the *-rep-num.chr.fit created by the bot
            #print 'skipping', f
            continue

        file_name, file_extension = os.path.splitext(f)
        fitness_in_file = float(open(f).read().strip())

        dirname = os.path.dirname(f)
        #print file_name
        #files_pattern = os.path.join(dirname, "%d-rep-*.res.xml" % file_name)
        files_pattern = "%s-rep-*.res.xml" % file_name
        #print files_pattern
        files = glob.glob(files_pattern)
        victories = 0
        for f in files:
            xml_tree = ET.parse(f).getroot()
            if xml_tree.find('result').get('value') == 'win':
                victories += 1

        fitness_in_results = float(victories) / len(files)
        if abs(fitness_in_file - fitness_in_results) > 0.005:  # uses a small tolerance
            #print '.fit says %f and .xml.res says %f for for %s' % (fitness_in_file, fitness_in_results, f)
            invalids += 1

            if fitness_in_results > 1:
                print 'ERROR: fitness > 1 in %s' % file_name

        elif fitness_in_results > 1:
            print 'ERROR: fitness > 1 in %s' % file_name
            invalids +=1

        #else:
         #   sys.stdout.write('\rfile = %.5f, results = %.5f' % (fitness_in_file, fitness_in_results))

    return invalids


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Adds the results of an experiment to a .zip file '
                                                 'And commits it to git repository.')

    parser.add_argument('paths', metavar='path', type=str, nargs='*',
                       help='Path to root directory of experiment results')

    parser.add_argument('-f', '--fitness', type=str, # default='unit_score',
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
