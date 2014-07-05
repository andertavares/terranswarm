import os
import re
import shutil

def inicopy(origin_ini):
    '''
    Copies origin_ini into bwapi .ini in starcraft dir
    :param origin_ini:
    :return:
    '''
    sc_dir, cl_path = read_paths()

    try:
        exp_inipath = os.path.join('setup', origin_ini)
        sc_inipath = os.path.join(sc_dir, 'bwapi-data', 'bwapi.ini')
        shutil.copyfile(exp_inipath, sc_inipath)
    except IOError:
        print 'An error has occurred. Could not copy %s \n' \
              'to %s' % (exp_inipath, sc_inipath)
        exit()

def read_paths():
    #read from paths.ini
    paths = open('paths.ini', 'r').read()

    sc_dir = re.match(r'.*starcraft_dir.?=.?"(.*?)"', paths, re.M | re.I | re.DOTALL).group(1)
    #sc_dir = sc_match_obj.group(1)

    cl_path = re.match(r'.*chaoslauncher_path.?=.?"(.*?)"', paths, re.M | re.I | re.DOTALL).group(1)

    if not os.path.exists(sc_dir):
        raise RuntimeError('Directory to Starcraft was not found. paths.ini says: %s' % sc_dir)

    if not os.path.exists(cl_path):
        raise RuntimeError('Chaoslauncher executable was not found. paths.ini says: %s' % cl_path)

    return (sc_dir, cl_path)