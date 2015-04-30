import os
import sys
import paths
import shutil
import time
import run as run
import run_best_fitness as best
import zip_and_add as zipadd

def runStep(configFile): #run.py
    #checks the existence of the AI module:
    if not paths.ai_module_exists():
        print 'AI module not found in %s' % paths.ai_module_path()
        print 'Please, compile the AI module and place it there.'
        print 'Exiting.'
        exit()

    #checks if map is in right place
    paths.check_map_location()
    run.runGA(configFile)

def bestStep(configFile, numMatches): #run_best_fitness.py
    print "\nThis step requires acess to C:\. If any errors ocurr, please run as admin\n"
    best.go(configFile, numMatches)

def zipStep(configFile): #zip_and_add.py
    zipadd.zip_and_add(configFile, 'git', False, False, False, False)

if __name__ == "__main__":

    if os.path.exists('paths.ini'):
        numConfigs = len(sys.argv) - 1
        numMatches = int(sys.argv[len(sys.argv) - 1])
        for n in range(1, numConfigs):
            runStep(sys.argv[n])    
            bestStep (sys.argv[n], numMatches)
            zipStep (sys.argv[n])
            time.sleep(2)
    else:
        #creates the file and fills it with default values
        print ('The file paths.ini was not found.')
        print ('We will generate a default one.')

        paths.ini_create()

        print ('Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system. Correct them if necessary and run\n' \
              'this script again. ')
