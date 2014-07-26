#!/usr/bin/env python
# a bar plot with errorbars

import numpy as np
import matplotlib.pyplot as plt

N = 3
randomPercents = 	(36, 30, 3.33)
manSWPercents 	= 	(93, 58, 54)
aiurPercents 	= 	(98, 75.33, 56)
swGAPPercents 	= 	(95, 74, 59)
swGAPePercents 	= 	(93.33, 62.67, 29.33)

ind = np.arange(N)  # the x locations for the groups
width = 0.15       	# the width of the bars
fig, ax = plt.subplots()

print ind+(width*0)
print ind+(width*1)
print ind+(width*2)

rects1 = ax.bar(ind+(width*0) + 0.13, randomPercents, width, color='#FFBE7B')
for bar in rects1:
	bar.set_hatch('||')

rects2 = ax.bar(ind+(width*1) + 0.13, manSWPercents, width, color='#FFB71E')
for bar in rects2:
	bar.set_hatch('\\')

rects4 = ax.bar(ind+(width*3) + 0.13, swGAPPercents, width, color='#4D060A', edgecolor='white')
for bar in rects4:
	bar.set_hatch('/')

rects3 = ax.bar(ind+(width*2) + 0.13, aiurPercents, width, color='#FF6900')
for bar in rects3:
	bar.set_hatch('.')

rects5 = ax.bar(ind+(width*4) + 0.13, swGAPePercents, width, color='#1A0001', edgecolor='white')
for bar in rects5:
	bar.set_hatch('x')

# add some
ax.set_ylabel('Victory rate %')
#ax.set_title('Scores by group and gender')
ax.set_xticks(ind+(width*3.5))
ax.set_xticklabels( ('Zerg', 'Terran', 'Protoss') )

ax.legend( (rects1[0], rects2[0], rects3[0], rects4[0], rects5[0]), ('Random', 'ManSW', 'AIUR', 'GASW', 'GASW-e'),loc=1, ncol=2)
ax.set_ylim([0,119])


def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(
        	rect.get_x()+rect.get_width()/2., 
        	height + 2, 
        	'%d'%int(height), 
        	ha='center', 
        	va='bottom'
        	)

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)
autolabel(rects4)
autolabel(rects5)

plt.show()