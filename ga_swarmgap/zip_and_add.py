import os
import paths
import sys
import shutil
import argparse
import subprocess
import configparser


def zip_and_add(config_file, commit_mode, no_pull=False, no_add=False, no_commit=False, no_set_user=False, no_push=False):
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

    # zips the output_dir
    zip_path = '%s.zip' % exp_out

    shutil.make_archive(exp_out, "zip", exp_out)
    
    # copies zip_file to results/
    try:
        copied_zip_path = os.path.join('results', '%s.zip' % config.output_dir)
        shutil.copyfile(zip_path, copied_zip_path)
    except IOError as e:
        print 'An error has occurred. Could not copy %s \n' \
              'to %s' % (zip_path, copied_zip_path)
        raise e

    if commit_mode == 'git':
        # performs a git pull
        if not no_pull:
            subprocess.call(['git', 'pull', 'origin', 'master'])
            print 'git pull attempted'

        # performs a git add
        if not no_add:
            subprocess.call(['git', 'add', copied_zip_path])
            print 'Attemped to add files to git'

            # performs a git commit
            if not no_commit:

                # sets user.name and user.email
                if not no_set_user:
                    subprocess.call(['git', 'config', 'user.name', '"GA Results"'])
                    subprocess.call(['git', 'config', 'user.email', 'results@ga.com'])
                    print 'Git user.name and user.email have been set as GA Results and results@ga.com'

                # effectively commits
                subprocess.call(['git', 'commit', '-m', 'Result file %s' % copied_zip_path])
                print 'git commit attempted'

                if not no_push:
                    print 'will try to git push. You should have a .netrc file on home directory'
                    subprocess.call(['git', 'push'])
                    print 'git push attempted'


    elif commit_mode == 'svn':
        # performs svn up
        if not no_pull:
            subprocess.call(['svn', 'up'])
            print 'svn up attempted'

        # performs a svn add
        if not no_add:
            subprocess.call(['svn', 'add', copied_zip_path])
            print 'Attemped to add files to svn'

            # performs a svn commit (only if add was performed before)
            if not no_commit:
                subprocess.call(['svn', 'commit', '-m', 'Result file %s' % copied_zip_path])
                print 'svn commit attempted'

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Adds the results of an experiment to a .zip file '
                                                 'And commits it to git repository.')

    parser.add_argument('config', metavar='num_configs', type=str, nargs='*',
                       help='Path to a config file of an experiment to be zipped and added.')

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

    args = parser.parse_args()
   
    for cfg in args.config:
        zip_and_add(cfg, args.commit_mode, args.no_pull, args.no_add, args.no_commit, args.no_set_user, args.no_push)
