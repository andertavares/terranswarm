import os
import paths
import shutil
import zipfile
import subprocess
import configparser

def zip_and_add(config_file, no_pull=False, no_add=False, no_commit=False):
    """
    Zips the files in output directory specified in config_file,
    copies it to our results/ directory and adds it to git and performs a commit.
    (git pull origin master is performed before add; git push must be manually performed)
    Parameters can be set to disable auto pull, add and commit
    :param config_file: path to the .xml config file
    :return:
    """

    config = configparser.ConfigParser(config_file)

    exp_out = paths.experiment_output_path(config)


    ### CODIGO PRA CRIAR O ARQUIVO .ZIP ###
    zip_path = '%s.zip' % exp_out
    zip_file = zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED)
    #TODO: <<<adicionar os arquivos no zip_file

    #copies zip_file to results/
    try:
        copied_zip_path = os.path.join('results', '%s.zip' % config.output_dir)
        shutil.copyfile(zip_path, copied_zip_path)
    except IOError as e:
        print 'An error has occurred. Could not copy %s \n' \
              'to %s' % (zip_path, copied_zip_path)
        raise e

    #performs a git pull
    if not no_pull:
        subprocess.call(['git', 'pull', 'origin', 'master'])

    #performs a git add
    if not no_add:
        subprocess.call(['git', 'add', copied_zip_path])

    #performs a git commit
    if not no_commit:

        subprocess.call(['git', 'commit', '-m', 'Result file %s' % copied_zip_path])
