import sys


f = open(sys.argv[1])
word = ""
count= 0
games = 0
fitness = 0.0
botScore = 0.0
enemyScore = 0.0
wins = 0
loss = 0

for char in f.read():
    if (char == "\n"):
        fitness = fitness + botScore / enemyScore
        botScore = 0.0
        enemyScore = 0.0
        count = 0
        games = games + 1
    else:
        if (char != ","):
            word = word + char
        else:
            count = count + 1
            if (word == "win"):
                wins = wins + 1
            if (word == "loss"):
                loss = loss + 1
                
            if (count > 4 and count < 8):
                botScore = botScore + float(word)
            if (count > 8):
                enemyScore = enemyScore + float(word)

            word = ""

print "number of games: %d" %games
print "wins:   %d" %wins
print "losses: %d" %loss
print "avgFitness: %f" %(fitness / games)
