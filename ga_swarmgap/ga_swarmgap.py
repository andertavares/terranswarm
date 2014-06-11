import sys
import ga_manager as ga
import configparser

if __name__ == '__main__':
    #parser = argparse.ArgumentParser(description='Runs genetic algorithm in a JSP instance')
    #parser.add_argument('-c','--config', help='File with experiment configuration', required=True)

    #args = vars(parser.parse_args())

    if len(sys.argv) < 2:
        print 'ERROR: experiment config. file not informed.'
        exit()

    config = configparser.ConfigParser(sys.argv[1])

    genetic = ga.start(config)

    print 'DONE.'
