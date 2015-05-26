'''
Makes GAMedicReadValues play with the hand-configured chromosome.

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


def go(enemy, num_matches):
    sc_dir, cl_path = paths.read_paths()

    best_file = 'setup/bestValues_manual.txt'
    dest = os.path.join(sc_dir, 'bwapi-data', 'bestValues.txt')
    shutil.copyfile(best_file, dest)
    print '%s copied to %s' % (best_file, dest)

    #puts the correct .ini into bwapi.ini
    paths.inicopy('bwapi_readValues_%s.ini' % enemy)

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
            print 'Removing <sc>/results.txt and restarting.'
            rfile.close()
            open(results_path, 'w').close()  # resets results.txt file
            chaosLauncher = subprocess.Popen([cl_path])
            last_read = 0
            #rwe.erase_best_values(sc_dir, current_experiment)
        #print len(res_lines)
    #terminates chaoslauncher and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    chaosLauncher.terminate()
    rfile.close()

    print 'Matches finished. Check the results in %s file.' % results_path


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Runs GAMedicReadValues bot with our bestValues_manual.txt')

    parser.add_argument('enemy', metavar='enemy', type=str, nargs=1,
                       help='Enemy race for the bot to play')

    parser.add_argument('num_matches', metavar='num_matches', type=int, nargs=1,
                       help='Number of matches to play')
    '''
    parser.add_argument('--no-pull', action='store_true', default=False,
                       help="Does not execute pull before the add/commit")

    parser.add_argument('--no-add', action='store_true', default=False,
                       help='Does not execute add (only zip the file)')

    parser.add_argument('--no-commit', action='store_true', default=False,
                       help='Does not perform a commit (automatically activated with --no-add).')

    parser.add_argument('--no-set-user', action='store_true', default=False,
                       help='Does not perform git config user.name and user.email (automatically activated with --no-add and --no-commit).')

    parser.add_argument('-m', '--commit-mode', type=str, default='git',
                       help='Commit mode (use svn or git commands), default=git')

    parser.add_argument('--no-push', action='store_true', default=False,
                       help="Does not execute push after the add/commit")
    '''
    args = parser.parse_args()
    #enemy = sys.argv[1] if len(sys.argv) > 1 else 'protoss'
    #num_matches = sys.argv[2] if len(sys.argv) > 2 else 30

    go(args.enemy[0], args.num_matches)