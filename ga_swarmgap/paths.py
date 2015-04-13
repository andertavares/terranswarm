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


def ini_create():
    """
    Creates the 'paths.ini' file with default values
    :return:
    """
    pfile = open('paths.ini','w')

    pfile.write(';please check whether the starcraft directory and path to chaoslauncher match the ones in your system\n')
    pfile.write('starcraft_dir = "C:\Program Files\Starcraft"\n')
    pfile.write('chaoslauncher_path = "C:\Program Files\BWAPI\Chaoslauncher\Chaoslauncher.exe"\n')

    pfile.close()


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


def ai_module_path():
    """
    Returns the path to the AI module
    :return:str
    """
    sc_dir, cl_path = read_paths()

    return os.path.join(sc_dir, 'bwapi-data', 'AI', 'GAMedicAIModule_release.dll')


def experiment_output_path(cfg_object):
    """
    Returns the full path to the experiment's output path specified
    in the config object
    :param cfg_object: ConfigParser
    :return: str
    """
    sc_dir, cl_path = read_paths()

    return os.path.join(sc_dir, cfg_object.output_dir)


def ai_module_exists():
    """
    Returns whether GAMedicAIModule_release.dll exists in <starcraft/bwapi-data/AI directory
    :return: bool
    """
    return os.path.exists(ai_module_path())


def check_map_location():
    """
    Checks whether Astral Balance_edit.scm is in starcraft maps.
    If not, it is copied there.
    :return:
    """

    sc_dir, cl_path = read_paths()
    sc_map_path = os.path.join(sc_dir, 'maps', 'Broodwar', '(2)Astral Balance_edit.scm')
    if not os.path.exists(sc_map_path):
        print 'Map %s not found, we will copy one there.' % sc_map_path

        try:
            our_map_path = os.path.join('setup', '(2)Astral Balance_edit.scm')
            shutil.copyfile(our_map_path, sc_map_path)
        except IOError as e:
            print 'An error has occurred. Could not copy %s \n' \
                  'to %s' % (our_map_path, sc_map_path)
            raise e

        print 'Copy successful.'