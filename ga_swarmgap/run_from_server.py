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

import argparse
import requests
import socket
import json
import HTMLParser
import datetime

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


def get_experiment_from_server(server_address):
    hostname = socket.gethostname()
    payload = {'server_name': hostname}

    if server_address[-1] == "/":
        server_address = server_address[:-1]

    server_address = server_address.replace("http://", "")
    server_address = server_address.replace("https://", "")

    decoded = {}

    try:
        server_uri = "http://" + server_address + "/ga_server/experiment/get_experiment"
        r = requests.post(server_uri, data=payload, timeout=5)
        decoded = json.loads(r.content)
    except requests.exceptions.Timeout:
        return {'success' : False, 'message': "Timeout exception "}
    except requests.exceptions.RequestException as e:
        return {'success' : False, 'message': "Request exception :" + str(e)}

    #print "Content:", r.content
    #print "json dump: ", json.dumps(decoded, sort_keys=True, indent=4)
    #print "json decoded:", decoded

    if "success" in decoded and decoded["success"] == "true":
        h = HTMLParser.HTMLParser()
        xml_decoded = h.unescape(decoded["xml_data"])

        #tf = tempfile.NamedTemporaryFile(delete=False)

        basename = "xml_from_server_" + decoded["name"]
        suffix = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
        filename = "_".join([basename, suffix]) + ".xml"
        full_file_path = "runs/server_experiment/" + filename
        #print "Xml decoded: ", xml_decoded
        print "Temp file: ", full_file_path

        if not os.path.exists(os.path.dirname(full_file_path)):
            os.makedirs(os.path.dirname(full_file_path))

        with open(full_file_path, "w") as text_file:
            text_file.write(xml_decoded)

        return {'success' : True, 'xml_filepath' : full_file_path, 'num_matches': decoded["num_matches"], "experiment_id": decoded["id"], "name": decoded["name"]}
    else:
        return {'success' : False }


def set_experiment_status(server_address, experiment_id, experiment_status):
    if server_address[-1] == "/":
        server_address = server_address[:-1]

    server_address = server_address.replace("http://", "")
    server_address = server_address.replace("https://", "")

    payload = {'experiment_id': experiment_id, 'experiment_status': experiment_status}

    try:
        server_uri = "http://" + server_address + "/ga_server/experiment/set_experiment_status"
        r = requests.post(server_uri, data=payload, timeout=5)
        decoded = json.loads(r.content)
    except requests.exceptions.Timeout:
        return {'success' : False, 'message': "Timeout exception "}
    except requests.exceptions.RequestException as e:
        return {'success' : False, 'message': "Request exception :" + str(e)}

    if "success" in decoded and decoded["success"] == "true":
        return {'success' : True}
    else:
        return {'success' : False, 'message': decoded["message"]}


if __name__ == "__main__":

    if not os.path.exists('paths.ini'):
        paths.ini_create()
        print ('The file paths.ini was not found.')
        print ('We will generate a default one.')
        print ('Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system.')

    parser = argparse.ArgumentParser(description='Run experiments from a specified server')

    parser.add_argument('-s', '--server', help="The server to query the experiments", required=True)
    parser.add_argument('-l', '--loop', action='store_true', help="Define if the server is going to be on a loop waiting for jobs", required=False)
    
    args = parser.parse_args()

    print 'Server address:', args.server
    print 'Is looping activated:', args.loop
    if not args.loop:
        print "    **** ALERT, looping to get new experiment is NOT ACTIVATED ****    "

    while True:
        return_experiment = get_experiment_from_server(args.server)
        if return_experiment["success"] == True:
            print "\n------------------8<------------------"
            print "Experiment Name: ", return_experiment["name"]
            print "Experiment ID: ", return_experiment["experiment_id"]
            print "Experiment XML path: ", return_experiment["xml_filepath"]
            print "Experiment Num runs:", return_experiment["num_matches"]
            set_status_result = set_experiment_status(args.server, return_experiment["experiment_id"], "RUNNING")
            print "Set status to RUNNING: ", set_status_result["success"]
            
            run_step(return_experiment["xml_filepath"])
            best_step(return_experiment["xml_filepath"], int(return_experiment["num_matches"]))
            zip_step(return_experiment["xml_filepath"])
            time.sleep(2)

            set_status_result = set_experiment_status(args.server, return_experiment["experiment_id"], "FINISHED")
            print "Set status to FINISHED: ", set_status_result["success"]
            print "------------------8<------------------\n"
            time.sleep(2)
        else:
            print "No experiment found on server, retrying..", time.strftime("%Y-%m-%d %H:%M:%S")
            if "message" in return_experiment:
                print "    ", return_experiment["message"], time.strftime("%Y-%m-%d %H:%M:%S") 

        time.sleep(4)
        if not args.loop:
            print "No looping flag, exiting.."
            break

    exit()

