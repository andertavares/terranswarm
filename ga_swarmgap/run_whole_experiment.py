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


def is_responding(name):
    #get process running with that name and divide file in lines
    r = os.popen('tasklist /FI "IMAGENAME eq %s" /FI "STATUS eq running"' % name).read().strip().split('\n')
    #see if the process runnin is the parameter one (return true)
    #or the file sent error mensage (return false)
    #print r

    if r == ['']:  # prevents error when process is not found(?)
        return False

    #print r
    #print r[-1].split()[0]
    return r[-1].split()[0].lower() == name.lower()
    '''if r[-1].split()[0] == name:
        return True
    else:
        return False'''


def start_script(file_path):
    print 'Restarting %s' % sys.executable

    print file_path
    os.execv(file_path, ['"%s"' % file_path] + sys.argv)
    #os.execv('python', ['"%s"' % file_path] + sys.argv)
    exit()


def erase_best_values(sc_dir, cfg_file):
    cfg = configparser.ConfigParser(cfg_file)
    experiment_path = os.path.join(sc_dir, cfg.output_dir)
    bestval_file = os.path.join(experiment_path, "bestValues.txt")

    if os.path.exists(bestval_file):
        os.remove(bestval_file)


def monitor_once():
    """
    This function is an attempt to solve the sc-start-kill confusion
    """
    #print 'monitoring once...'
    #time.sleep(3) #check every 10s

    sc_dir, cl_path = paths.read_paths()
    error_dir = os.path.join(sc_dir, "Errors")  #get path to Error dir
    error_files = os.listdir(error_dir)         #get error files
    big_error = False
    max_size = 100000000    # 100mb

    #check the existance of error with size > max_size
    for err_file in error_files:
        if os.stat(os.path.join(error_dir, err_file)).st_size > max_size:
            big_error = True
            big_file = os.path.join(error_dir, error_files[n])
            break

    #check if both SC and CL are running (or neither)
    #print 'sc %s, cl %s, ^ %s' % (is_responding("starcraft.exe"), is_responding("Chaoslauncher.exe"), is_responding("starcraft.exe") ^ is_responding("Chaoslauncher.exe"))
    sc_crashed = is_responding("starcraft.exe") ^ is_responding("Chaoslauncher.exe")

    if sc_crashed or big_error:
        print 'StarCraft has crashed or got lost writing error file.'
        #should restart. Kill both starcraft and chaoslaucher
        print 'Closing StarCraft...'
        subprocess.call("taskkill /IM starcraft.exe")
        time.sleep(5) #give os some time to close programs

        print 'Closing Chaoslauncher...'
        subprocess.call("taskkill /IM Chaoslauncher.exe")
        time.sleep(5) #give os some time to close programs

        #erase big error file
        if big_error:
            print 'Erasing big error file.'
            os.remove(big_file)

        #erase bestValues.txt if it exists (causes error if he is there)
        #erase_best_values(sc_dir, current_experiment)

        return False  # indicates that an error has ocurred
        #restart
        #start_script(sys.executable)
    return True  # indicates that things are fine


def monitor():
    while True:
        print 'monitoring...'
        time.sleep(1) #check every 10s

        sc_dir, cl_path = paths.read_paths()
        error_dir = os.path.join(sc_dir, "Errors")  #get path to Error dir
        error_files = os.listdir(error_dir)         #get error files
        big_error = False
        max_size = 100000000    # 100mb

        #check the existance of error with size > max_size
        for err_file in error_files:
            if os.stat(os.path.join(error_dir, err_file)).st_size > max_size:
                big_error = True
                big_file = os.path.join(error_dir, error_files[n])
                break

        #check if both SC and CL are running (or neither)
        #print 'sc %s, cl %s, ^ %s' % (is_responding("starcraft.exe"), is_responding("Chaoslauncher.exe"), is_responding("starcraft.exe") ^ is_responding("Chaoslauncher.exe"))
        sc_crashed = is_responding("starcraft.exe") ^ is_responding("Chaoslauncher.exe")
        
        if sc_crashed or big_error:
            print 'StarCraft has crashed or got lost writing error file.'
            #should restart. Kill both starcraft and chaoslaucher
            print 'Closing StarCraft...'
            subprocess.call("taskkill /IM starcraft.exe")
            time.sleep(5) #give os some time to close programs

            print 'Closing Chaoslauncher...'
            subprocess.call("taskkill /IM Chaoslauncher.exe")
            time.sleep(5) #give os some time to close programs

            #erase big error file 
            if big_error:
                print 'Erasing big error file.'
                os.remove(big_file)

            #erase bestValues.txt if it exists (causes error if he is there)
            #erase_best_values(sc_dir, current_experiment)

            #restart script
            start_script(sys.executable)

    
def run_step(config_file): #run.py
    #checks the existence of the AI module:
    if not paths.ai_module_exists():
        print 'AI module not found in %s' % paths.ai_module_path()
        print 'Please, compile the AI module and place it there.'
        print 'Exiting.'
        exit()

    #checks if map is in right place
    paths.check_map_location()
    run.runGA(config_file)


def best_step(config_file, num_matches): #run_best_fitness.py
    print "\nThis step requires access to C:\. If any errors ocurr, please run as admin\n"
    best.go(config_file, num_matches)


def zip_step(config_file): #zip_and_add.py
    zipadd.zip_and_add(config_file, 'git', False, False, False, False)


if __name__ == "__main__":

    #thread.start_new_thread(monitor, ())

    if os.path.exists('paths.ini'):
        numConfigs = len(sys.argv) - 1
        numMatches = int(sys.argv[len(sys.argv) - 1])
        for n in range(1, numConfigs):
            current_experiment = sys.argv[n]
            run_step(current_experiment)
            best_step(current_experiment, numMatches)
            zip_step(current_experiment)
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

