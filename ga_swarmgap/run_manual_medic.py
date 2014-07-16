'''
Makes GAMedicReadValues play with the hand-configured chromosome.

'''
import configparser
import subprocess
import shutil
import paths
import time
import sys
import os
import re

def go(enemy, num_matches):
    sc_dir, cl_path = paths.read_paths()

    best_file = 'setup/bestValues_manual.txt'
    dest = "c:/bestValues.txt"
    shutil.copyfile(best_file, dest)
    print '%s copied to %s' % (best_file, dest)

    #puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_readValues_%s.ini' % enemy)

    #checks if result.txt file exists:
    results_path = os.path.join(sc_dir, 'results.txt')

    if os.path.exists(results_path):

        i = 1
        while True:
            tentative_bkp_name = 'results_old%d.txt' % i
            if not os.path.exists(os.path.join(sc_dir, tentative_bkp_name)):
                break
            i += 1

        print 'WARNING: a previous results.txt exists. It will be renamed to %s.\n' \
              'A new results.txt will be created with the results will soon appear below.' % tentative_bkp_name
        os.rename(results_path, os.path.join(sc_dir, tentative_bkp_name))
        open(results_path, 'w').close() #create empty results.txt file

    #calls chaoslauncher and monitors results.txt
    chaosLauncher = subprocess.Popen([cl_path])
    last_read = 0
    while True:
        rfile = open(results_path)
        res_lines = rfile.readlines()

        if len(res_lines) > last_read:
            print 'Result for match %d: %s' % (len(res_lines), res_lines[-1].strip())
            last_read = len(res_lines)

        if len(res_lines) >= num_matches:
            break
        time.sleep(1)
        #print len(res_lines)
    #terminates chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    chaosLauncher.terminate()

    print 'Matches finished. Check the results in %s file.' % results_path


if __name__ == '__main__':
    enemy = sys.argv[1] if len(sys.argv) > 1 else 'protoss'
    num_matches = sys.argv[2] if len(sys.argv) > 2 else 30

    go(enemy, int(num_matches))