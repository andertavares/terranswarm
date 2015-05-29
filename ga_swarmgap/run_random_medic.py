"""
Runs RandomMedicAIModule_release.dll in a specified # of matches against a specified enemy.

"""
import run_whole_experiment as rwe
import configparser
import subprocess
import argparse
import shutil
import paths
import time
import sys
import os
import re


def go(enemy, num_matches, output):
    sc_dir, cl_path = paths.read_paths()

    if os.path.exists(output):
        print 'File %s already exists and won\'t be overwritten. Exiting...'
        exit()

    #puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_randomMedic_%s.ini' % enemy)

    # copies setup/RandomMedicAIModule_release.dll to <starcraft>/bwapi-data/AI
    try:
        our_ai_dllpath = os.path.join('setup', 'RandomMedicAIModule_release.dll')
        sc_ai_dllpath = os.path.join(sc_dir, 'bwapi-data', 'AI', 'RandomMedicAIModule_release.dll')
        shutil.copyfile(our_ai_dllpath, sc_ai_dllpath)

    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
                'to %s' % (our_ai_dllpath, sc_ai_dllpath)
        exit()


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
        time.sleep(2)

        if not rwe.monitor_once(): #problem... relaunch starcraft
            #print 'Removing <sc>/results.txt and restarting.'
            #rfile.close()
            #open(results_path, 'w').close()  # resets results.txt file
            chaosLauncher = subprocess.Popen([cl_path])
            last_read = 0

    # experiment finished; terminates chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    chaosLauncher.terminate()
    rfile.close()

    #moves/renames results.txt to the name specified by the user
    os.rename(results_path, output)

    print 'Matches finished. Check the results in %s file.' % output


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Runs RandomMedic bot against the specified enemy.')

    parser.add_argument('enemy', metavar='enemy', type=str, #nargs=1,
                       help='Enemy race for the bot to play')

    parser.add_argument('num_matches', metavar='num_matches', type=int, #nargs=1,
                       help='Number of matches to play')

    parser.add_argument('-o', '--output', required=True,
                       help="Output file with the results")

    args = parser.parse_args()

    go(args.enemy, args.num_matches, args.output)