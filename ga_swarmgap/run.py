'''
Runs the program

'''
import os
import sys
import ga_manager as ga

def runGA(config_file):
    print ('Genetic algorithm for SwarmGAP in Starcraft started.')
    ga.start(config_file)
    print ('DONE.')

if __name__ == "__main__":
    #checks whether paths.ini already exists:
    if os.path.exists('paths.ini'):
		numConfigs = len(sys.argv)
		for n in range(1,numConfigs):
			runGA(sys.argv[n])

    else: #creates the file and fills it with default values
        print ('The file paths.ini was not found.')
        print ('We will generate a default one.')

        pfile = open('paths.ini','w')

        pfile.write(';please check whether the starcraft directory and path to chaoslauncher match the ones in your system\n')
        pfile.write('starcraft_dir = "E:\Starcraft"\n')
        pfile.write('chaoslauncher_path = "C:\Program Files\BWAPI\Chaoslauncher\Chaoslauncher.exe"\n')

        pfile.close()

        print ('Done. Please check in paths.ini whether the starcraft directory and path to \n' \
              'chaoslauncher match the ones in your system. Correct them if necessary and run\n' \
              'this script again. ')
