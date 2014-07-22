'''
Check if starcraft is running and if not spawn it again

'''
import configparser
import subprocess
import win32ui
import shutil
import paths
import time
import sys
import os
import re


def isWindowOpen(classname, classname2):
    try:
        win32ui.FindWindow(classname, classname2)
    except win32ui.error:
        return False
    else:
        return True

def go():
    #sc_dir, cl_path = paths.read_paths()

    #calls chaoslauncher
    os.chdir( "C:\Program Files (x86)\BWAPI\Chaoslauncher\\" )
    chaosLauncher = subprocess.Popen("C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher - MultiInstance.exe")
    time.sleep(3)

    while True:
        if(isWindowOpen(None, "StarCraft")):
            print "error message open"
            subprocess.call("taskkill /IM WerFault.exe")
            time.sleep(1)
        elif(not isWindowOpen(None, "Brood War")):
            subprocess.call("taskkill /IM starcraft.exe")
            chaosLauncher.terminate()
            time.sleep(1)
            chaosLauncher = subprocess.Popen("C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher - MultiInstance.exe")
            time.sleep(3)
        time.sleep(2)

    #terminates chaoslauncher
    #  and starcraft
    subprocess.call("taskkill /IM starcraft.exe")
    chaosLauncher.terminate()

    print 'Script finished'


if __name__ == '__main__':
    go()