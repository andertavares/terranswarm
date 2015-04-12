'''
Runs the program

'''
import os
import sys
import paths
import shutil
import configparser
import ga_manager as ga

def runGA(config_file):
    """
    Runs one instance of the genetic algorithm
    :param config_file: (str) path to a .xml configuration file
    :return:
    """

    config = configparser.ConfigParser(config_file)
    print 'GA for SwarmGAP in Starcraft started. Config: %s' % config_file
    print 'Configurations:'

    print 'Output dir: %s '% config.output_dir
    print 'Enemy: %s' % config.enemy
    print 'p_crossover:%s' % config.p_crossover
    print 'p_mutation: %s'% config.p_mutation
    print 'Generations: %s'% config.generations
    print 'pop. size: %s' % config.popsize
    print 'Tournament size: %s' % config.tournament_size
    print 'Elitism? %s' % config.elitism
    print 'Reliab. threshold: %s' % config.reliab_threshold
    print 'p_eval_above_threshold: %s' % config.p_eval_above_thresh
    print 'Random seed: %s' % config.random_seed

    ga.start(config)
    print 'DONE.'



if __name__ == "__main__":
    #checks whether paths.ini already exists:
    if os.path.exists('paths.ini'):
        #checks the existence of the AI module:
        if not paths.ai_module_exists():
            print 'AI module not found in %s' % paths.ai_module_path()
            print 'Please, compile the AI module and place it there.'
            print 'Exiting.'
            exit()

        #checks if map is in right place
        paths.check_map_location()

        numConfigs = len(sys.argv)

        for n in range(1, numConfigs):
            runGA(sys.argv[n])

    else: #creates the file and fills it with default values
        print ('The file paths.ini was not found.')
        print ('We will generate a default one.')

        paths.ini_create()

        print ('Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system. Correct them if necessary and run\n' \
              'this script again. ')
