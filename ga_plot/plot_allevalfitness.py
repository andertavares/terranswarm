#!/usr/bin/env python
# a bar plot with errorbars

import sys
import numpy as np
import matplotlib.pyplot as plt

N = 3
ind = np.arange(N) # the x locations for the groups
width = 0.09 # the width of the bars
fig, ax = plt.subplots()

f = open(sys.argv[1], 'r')

race = f.readline()

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

f11=float(f.readline().replace(',','.'))/1.5
f12=float(f.readline().replace(',','.'))/1.5
f13=float(f.readline().replace(',','.'))/1.5
f14=float(f.readline().replace(',','.'))/1.5
f15=float(f.readline().replace(',','.'))/1.5
f16=float(f.readline().replace(',','.'))/1.5
f17=float(f.readline().replace(',','.'))/1.5
f18=float(f.readline().replace(',','.'))/1.5
f19=float(f.readline().replace(',','.'))/1.5
f20=float(f.readline().replace(',','.'))/1.5

e11=float(f.readline().replace(',','.'))/1.5
e12=float(f.readline().replace(',','.'))/1.5
e13=float(f.readline().replace(',','.'))/1.5
e14=float(f.readline().replace(',','.'))/1.5
e15=float(f.readline().replace(',','.'))/1.5
e16=float(f.readline().replace(',','.'))/1.5
e17=float(f.readline().replace(',','.'))/1.5
e18=float(f.readline().replace(',','.'))/1.5
e19=float(f.readline().replace(',','.'))/1.5
e20=float(f.readline().replace(',','.'))/1.5

f21=float(f.readline().replace(',','.'))/1.5
f22=float(f.readline().replace(',','.'))/1.5
f23=float(f.readline().replace(',','.'))/1.5
f24=float(f.readline().replace(',','.'))/1.5
f25=float(f.readline().replace(',','.'))/1.5
f26=float(f.readline().replace(',','.'))/1.5
f27=float(f.readline().replace(',','.'))/1.5
f28=float(f.readline().replace(',','.'))/1.5
f29=float(f.readline().replace(',','.'))/1.5
f30=float(f.readline().replace(',','.'))/1.5

e21=float(f.readline().replace(',','.'))/1.5
e22=float(f.readline().replace(',','.'))/1.5
e23=float(f.readline().replace(',','.'))/1.5
e24=float(f.readline().replace(',','.'))/1.5
e25=float(f.readline().replace(',','.'))/1.5
e26=float(f.readline().replace(',','.'))/1.5
e27=float(f.readline().replace(',','.'))/1.5
e28=float(f.readline().replace(',','.'))/1.5
e29=float(f.readline().replace(',','.'))/1.5
e30=float(f.readline().replace(',','.'))/1.5

r1valueMeans = (f1,f11,f21)
r1valueErr =  (e1,e11,e21)

r2valueMeans = (f2,f12,f22)
r2valueErr =  (e2,e12,e22)

r3valueMeans = (f3,f13,f23)
r3valueErr =  (e3,e13,e23)

r4valueMeans = (f4,f14,f24)
r4valueErr =  (e4,e14,e24)

r5valueMeans = (f5,f15,f25)
r5valueErr =  (e5,e15,e25)

r6valueMeans = (f6,f16,f26)
r6valueErr =  (e6,e16,e26)

r7valueMeans = (f7,f17,f27)
r7valueErr =  (e7,e17,e27)

r8valueMeans = (f8,f18,f28)
r8valueErr =  (e8,e18,e28)

r9valueMeans = (f9,f19,f29)
r9valueErr =  (e9,e19,e29)

r10valueMeans = (f10,f20,f30)
r10valueErr =  (e10,e20,e30)

rects1 = ax.bar(ind+(width/2), r1valueMeans, width, color='darkblue', yerr=r1valueErr)
rects2 = ax.bar(ind+width+(width/2), r2valueMeans, width, color='darkblue', yerr=r2valueErr)  
rects3 = ax.bar(ind+2*width+(width/2), r3valueMeans, width, color='darkblue', yerr=r3valueErr)
rects4 = ax.bar(ind+3*width+(width/2), r4valueMeans, width, color='darkblue', yerr=r4valueErr)
rects5 = ax.bar(ind+4*width+(width/2), r5valueMeans, width, color='darkblue', yerr=r5valueErr)
rects6 = ax.bar(ind+5*width+(width/2), r6valueMeans, width, color='darkblue', yerr=r6valueErr)
rects7 = ax.bar(ind+6*width+(width/2), r7valueMeans, width, color='darkblue', yerr=r7valueErr)
rects8 = ax.bar(ind+7*width+(width/2), r8valueMeans, width, color='darkblue', yerr=r8valueErr)
rects9 = ax.bar(ind+8*width+(width/2), r9valueMeans, width, color='darkblue', yerr=r9valueErr)
rects10 = ax.bar(ind+9*width+(width/2), r10valueMeans, width, color='darkblue', yerr=r10valueErr)


# add some text for labels, title and axes ticks
ax.set_ylabel('Victories (%)')

#ax.set_title('Percentage of victories by enemy race and fitness fuction')
ax.set_xticks(ind+width)

race=" "
ax.set_xticklabels( ('        Protoss', '        Terran', '     Zerg'))
#ax.legend( (rects1[0], rects2[0], rects3[0]), ('Terran', 'Protoss', 'Zerg') )

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
autolabel(rects5)
autolabel(rects6)
autolabel(rects7)
autolabel(rects8)
autolabel(rects9)
autolabel(rects10)
#autolabel(rects4)
plt.show()