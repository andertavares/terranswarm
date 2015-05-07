import os
import sys
import paths
import shutil
import time
import run as run
import run_best_fitness as best
import zip_and_add as zipadd
import thread
import subprocess
import configparser

global current_experiment

def isresponding(name):
    #get process running with that name and divide file in lines
    r = os.popen('tasklist /FI "IMAGENAME eq %s" /FI "STATUS eq running"' % name).read().strip().split('\n')

    #see if the process runnin is the parameter one (return true)
    #or the file sent error mensage (return false)
    if r[len(r)-1].split()[0] == name:
        return True
    else:
        return False    

def startScript(filePath):
    print 'Restarting %s' %sys.executable
    os.execv(filePath, [filePath] + sys.argv)
    exit()

def eraseBestValues(sc_dir, cfg_file):
    cfg = configparser.ConfigParser(cfg_file)
    experiment_path = os.path.join(sc_dir, cfg.output_dir)
    bestval_file = os.path.join(experiment_path, "bestValues.txt")
    if os.path.exists(bestval_file):
        os.remove(bestval_file)


def monitor():
    while(True):        
        time.sleep(60) #check every minute

        sc_dir, cl_path = paths.read_paths()
        error_dir = os.path.join(sc_dir, "Errors")  #get path to Error dir
        error_files = os.listdir(error_dir)         #get error files
        big_error=False
        max_size = 100000000    #100mb

        #check the existance of error with size > max_size
        for n in range (0, len(error_files)):       
            if (os.stat(os.path.join(error_dir, error_files[n])).st_size > max_size):
                big_error = True
                big_file = os.path.join(error_dir, error_files[n])
                break

        #check if both SC and CL are running (or neither) 
        sc_crashed = isresponding("starcraft.exe") ^ isresponding("Chaoslauncher.exe")
        
        if (sc_crashed or big_error):          
            #should restart. Kill both starcraft and chaoslaucher
            subprocess.call("taskkill /IM starcraft.exe")
            time.sleep(2) #give os sometime to close programs

            subprocess.call("taskkill /IM Chaoslauncher.exe")
            time.sleep(2) #give os sometime to close programs

            #erase big error file 
            if(big_error):
                os.remove(big_file)

            #erase bestValues.txt if it exists (causes error if he is there)
            eraseBestValues(sc_dir, current_experiment)

            #restart script
            startScript(sys.executable)    
    
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

    thread.start_new_thread(monitor, ())

    if os.path.exists('paths.ini'):
        numConfigs = len(sys.argv) - 1
        numMatches = int(sys.argv[len(sys.argv) - 1])
        for n in range(1, numConfigs):
            current_experiment = sys.argv[n]
            runStep(current_experiment)    
            bestStep(current_experiment, numMatches)
            zipStep(current_experiment)
            time.sleep(2)
    else:
        #creates the file and fills it with default values
        print ('The file paths.ini was not found.')
        print ('We will generate a default one.')

        paths.ini_create()

        print ('Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system. Correct them if necessary and run\n' \
              'this script again. ')
    exit()

