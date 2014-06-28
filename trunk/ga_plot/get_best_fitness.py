import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import re

rootdir = sys.argv[1]

fitnessDict = dict()

for subdir, dirs, files in os.walk(rootdir):
	
	#print "Actual dir:", subdir

	for file in files:
		fileName, fileExtension = os.path.splitext(file)
		fullPath = os.path.join(subdir, file)
		baseGeneration = os.path.basename(subdir)

		if baseGeneration != "":
			generationNumber = int(re.findall(r'\d+', baseGeneration)[0])

			if fileExtension == ".fit" :
					f = open(fullPath)
					fitness = float("".join(f.readlines()))
					f.close()
					#print '\t', generationNumber, file , fitness

					fitnessDict[generationNumber] = subdir

'''
The dict fitnessDict uses the integer number of the generation as the key
to hold an array of values (the float values of the fitness).
'''

lastGeneration = fitnessDict.keys()[len(fitnessDict.keys()) -1]
print "Last generation:", lastGeneration

#lastGeneration = 36

# TODO: Optimize for less uglyness

bestFitnessValue = 0
bestFitnessNumber = None

for subdir, dirs, files in os.walk(fitnessDict[lastGeneration]):

	for file in files:
		fileName, fileExtension = os.path.splitext(file)
		fullPath = os.path.join(subdir, file)
		baseGeneration = os.path.basename(subdir)

		if fileExtension == ".fit" :
			f = open(fullPath)
			fitness = float("".join(f.readlines()))
			f.close()
			if (fitness > bestFitnessValue):
				bestFitnessValue = fitness
				bestFitnessNumber = int(re.findall(r'\d+', file)[0])
				#print bestFitnessValue, bestFitnessNumber, file
			#print fitness, int(re.findall(r'\d+', file)[0]), fullPath

print "Fitness value:", bestFitnessValue, "Position of fitness:", bestFitnessNumber, "\n"
f = open(str(fitnessDict[lastGeneration])+"/"+str(bestFitnessNumber)+".chr.lock")
print "".join(f.readlines())
f.close()
#for key in fitnessDict.keys():
#	print key, '-->', fitnessDict[key]
