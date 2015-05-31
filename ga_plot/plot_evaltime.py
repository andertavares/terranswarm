#!/usr/bin/env python
# a bar plot with errorbars

import sys
import numpy as np
import matplotlib.pyplot as plt

N = 10
ind = np.arange(N) # the x locations for the groups
width = 0.9 # the width of the bars
fig, ax = plt.subplots()

f = open(sys.argv[1], 'r')

race = f.readline()

f1=float(f.readline().replace(',','.')) 
f2=float(f.readline().replace(',','.')) 
f3=float(f.readline().replace(',','.')) 
f4=float(f.readline().replace(',','.')) 
f5=float(f.readline().replace(',','.')) 
f6=float(f.readline().replace(',','.')) 
f7=float(f.readline().replace(',','.')) 
f8=float(f.readline().replace(',','.')) 
f9=float(f.readline().replace(',','.')) 
f10=float(f.readline().replace(',','.')) 

e1=float(f.readline().replace(',','.')) 
e2=float(f.readline().replace(',','.')) 
e3=float(f.readline().replace(',','.')) 
e4=float(f.readline().replace(',','.')) 
e5=float(f.readline().replace(',','.')) 
e6=float(f.readline().replace(',','.')) 
e7=float(f.readline().replace(',','.')) 
e8=float(f.readline().replace(',','.')) 
e9=float(f.readline().replace(',','.')) 
e10=float(f.readline().replace(',','.')) 

valueMeans = (f1,f2,f3,f4,f5,f6,f7,f8,f9,f10)
valueErr =  (e1,e2,e3,e4,e5,e6,e7,e8,e9,e10)

if(race=="Terran\n"):
  rects1 = ax.bar(ind+(width/2), valueMeans, width, color='darkblue', yerr=valueErr)
elif (race=="Protoss\n"):
  rects1 = ax.bar(ind+(width/2), valueMeans, width, color='g', yerr=valueErr)  
elif (race=="Zerg\n"):
  rects1 = ax.bar(ind+(width/2), valueMeans, width, color='darkred', yerr=valueErr)
else:
  exit()
  
# add some text for labels, title and axes ticks
ax.set_ylabel('Time (seconds)')

#ax.set_title('Percentage of victories by enemy race and fitness fuction')
ax.set_xticks(ind+width)

ax.set_xticklabels( ('0.1', '0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0'), rotation='vertical' )
#ax.legend( (rects1[0], rects2[0], rects3[0], rects4[0]), ('Score ratio', 'Time-based', 'Unit-based', 'Victory rate') )

def autolabel(rects):
  # attach some text labels
  for rect in rects:
    height = rect.get_height()
    ax.text(rect.get_x()+rect.get_width()/2., 1.01 * height, '%.1f' % float(height),
    ha='center', va='bottom')

autolabel(rects1)
#autolabel(rects2)
#autolabel(rects3)
#autolabel(rects4)
plt.show()
