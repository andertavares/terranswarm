'''
Plots the ammount of equal individuals along the generations

'''
import os
import sys
import glob
import numpy as np
import matplotlib.pyplot as plt

basedir = sys.argv[1]

generations = len(glob.glob(os.path.join(basedir, '*')))

print 'Found %d generations.' % generations

#counts the number of equal individuals along the generations
equal_array = []

for i in range(1, generations+1):
    equal_count = 0 #counts the number of equal individuals in this generations

    #retrieves all the chromosome files
    chromo_files = glob.glob(os.path.join(basedir, 'g%d' % i, '*.chr')) + glob.glob(os.path.join(basedir, 'g%d'  % i, '*.chr.lock'))
    #print len(chromo_files)#, os.path.join(basedir, 'g%d', '*.chr')
    #traverse the chromosome files, making pairwise comparison for equality of content
    for c1 in range(0, len(chromo_files) - 1):
        for c2 in range(c1+1, len(chromo_files)):
            if open(chromo_files[c1]).read() == open(chromo_files[c2]).read():
                equal_count += 1
            #else:
                #print '%s != %s' % (chromo_files[c1], chromo_files[c2])
    equal_array.append(equal_count)

    print 'Generation %d analysed.' % i

plt.plot(range(1, generations+1), equal_array, 'b', label='#equal')
#plt.plot(range(1, generations+1), [len(chromo_files)] * generations,'y--',label='total')
plt.legend()
plt.show()
