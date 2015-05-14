#!/usr/bin/env python
# a bar plot with errorbars
import sys
import numpy as np
import matplotlib.pyplot as plt

N = 3
ind = np.arange(N)  # the x locations for the groups
width = 0.15       # the width of the bars
fig, ax = plt.subplots()

f = open(sys.argv[1], 'r')

f1=float(f.readline().replace(',','.'))/1.5
f2=float(f.readline().replace(',','.'))/1.5
f3=float(f.readline().replace(',','.'))/1.5
f4=float(f.readline().replace(',','.'))/1.5
f5=float(f.readline().replace(',','.'))/1.5
f6=float(f.readline().replace(',','.'))/1.5
f7=float(f.readline().replace(',','.'))/1.5
f8=float(f.readline().replace(',','.'))/1.5
f9=float(f.readline().replace(',','.'))/1.5
f10=float(f.readline().replace(',','.'))/1.5
f11=float(f.readline().replace(',','.'))/1.5
f12=float(f.readline().replace(',','.'))/1.5

e1=float(f.readline().replace(',','.'))/1.5
e2=float(f.readline().replace(',','.'))/1.5
e3=float(f.readline().replace(',','.'))/1.5
e4=float(f.readline().replace(',','.'))/1.5
e5=float(f.readline().replace(',','.'))/1.5
e6=float(f.readline().replace(',','.'))/1.5
e7=float(f.readline().replace(',','.'))/1.5
e8=float(f.readline().replace(',','.'))/1.5
e9=float(f.readline().replace(',','.'))/1.5
e10=float(f.readline().replace(',','.'))/1.5
e11=float(f.readline().replace(',','.'))/1.5
e12=float(f.readline().replace(',','.'))/1.5



scoreMeans = (f3,f1,f2)
timeMeans = (f6,f4,f5)
unitMeans = (f9,f7,f8)
vicMeans = (f12,f10,f11)

scoreErr = (e3,e1,e2)
timeErr = (e6,e4,e5)
unitErr = (e9,e7,e8)
vicErr = (e12,e10,e11)


rects1 = ax.bar(ind + 0.05, scoreMeans, width, color='darkred', yerr=scoreErr)
rects2 = ax.bar(ind+width+0.08, timeMeans, width, color='goldenrod', yerr=timeErr)
rects3 = ax.bar(ind+2*width+0.11, unitMeans, width, color='darkblue', yerr=unitErr)
rects4 = ax.bar(ind+3*width+0.14, vicMeans, width, color='g', yerr=vicErr)


# add some text for labels, title and axes ticks
ax.set_ylabel('Victories (%)')
#ax.set_title('Percentage of victories by enemy race and fitness fuction')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('                  Zerg',  '                  Terran', '                  Protoss') )

ax.legend( (rects1[0], rects2[0], rects3[0], rects4[0]), ('Score ratio', 'Time-based', 'Unit-based', 'Victory ratio') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.01 * height, '%.1f' % float(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)
autolabel(rects4)

plt.show()
