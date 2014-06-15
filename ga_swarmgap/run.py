'''
Runs the program

'''
import os
import sys
import ga_manager as ga

def runGA(config_file):
    print 'Genetic algorithm for SwarmGAP in Starcraft started.'
    ga.start(config_file)
    print 'Done.'

if __name__ == "__main__":
    #checks whether paths.ini already exists:
    if os.path.exists('paths.ini'):
        runGA(sys.argv[1])

    else: #creates the file and fills it with default values
        print 'The file paths.ini was not found.'
        print 'We will generate a default one.'

        pfile = open('paths.ini','w')

        pfile.write(';please check whether the starcraft directory and path to chaoslauncher match the ones in your system\n')
        pfile.write('starcraft_dir = "C:/Program Files/Starcraft"\n')
        pfile.write('chaoslauncher_path = "C:/Program Files/Starcraft"\n')

        pfile.close()

        print 'Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system. Correct them if necessary and run\n' \
              'this script again. '
