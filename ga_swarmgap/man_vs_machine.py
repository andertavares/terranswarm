'''
Makes GAMedicReadValues play with a hand-configured chromosome against a Human.

We select a different 'bestValues.txt' according to the adversary.

- Terran is based on the results of terran_vratio-pop15-rep16.zip
- Protoss is based on the results of protoss_eval10-29.zip
- Zerg is based on the results of zerg_vratio-pop15-rep17.zip

'''
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


def go(enemy, output):
    sc_dir, cl_path = paths.read_paths()

    if os.path.exists(output):
        print 'File %s already exists and won\'t be overwritten. Please remove it. Exiting...' % output
        exit()

    best_file = 'setup/bestValues_%s.txt' % enemy
    dest = os.path.join(sc_dir, 'bwapi-data', 'bestValues.txt')

    shutil.copyfile(best_file, dest)
    print '%s copied to %s' % (best_file, dest)

    # puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_man-vs-machine.ini')

    # copies setup/GAMedicReadValues_release.dll to <starcraft>/bwapi-data/AI
    try:
        our_ai_dllpath = os.path.join('setup', 'GAMedicReadValues_release.dll')
        sc_ai_dllpath = os.path.join(sc_dir, 'bwapi-data', 'AI', 'GAMedicReadValues_release.dll')
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
    subprocess.call(cl_path)

    #terminates chaoslauncher and starcraft
    #subprocess.call("taskkill /IM starcraft.exe")
    #chaosLauncher.terminate()

    #moves/renames results.txt to the name specified by the user
    os.rename(results_path, output)

    print 'Matches finished. Check the result in %s.' % output


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Runs GAMedicReadValues for a match against a human.')

    parser.add_argument('enemy', metavar='enemy', type=str, #nargs=1,
                       help='Enemy race (the one chosen by the human)')

    #parser.add_argument('num_matches', metavar='num_matches', type=int, #nargs=1,
     #                  help='Number of matches to play')

    parser.add_argument('-o', '--output', required=True,
                       help="Output file with the results")

    args = parser.parse_args()

    go(args.enemy, args.output)