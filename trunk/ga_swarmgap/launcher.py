import subprocess
import time

CHAOSLAUNCHER = "C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher.exe"
STARCRAFT = ""
# os.startfile("C:\Program Files (x86)\BWAPI\Chaoslauncher\Chaoslauncher.exe")
from psutil import AccessDenied

chaosLauncher = subprocess.Popen([CHAOSLAUNCHER])


#for pid in psutil.pids():
#    proc = psutil.Process(pid)
#
#    try:
#        print proc.name()
#
#    except AccessDenied:
#        print "Access denied for pid=%d" % pid

time.sleep(10)

chaosLauncher.terminate()# send_signal(subprocess.SIGTERM)
subprocess.call("taskkill /IM starcraft.exe")