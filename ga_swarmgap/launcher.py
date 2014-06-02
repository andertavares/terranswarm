import subprocess
import time

CHAOSLAUNCHER = "C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher.exe"
STARCRAFT = ""

chaosLauncher = subprocess.Popen([CHAOSLAUNCHER])

time.sleep(10)

chaosLauncher.terminate()
subprocess.call("taskkill /IM starcraft.exe")