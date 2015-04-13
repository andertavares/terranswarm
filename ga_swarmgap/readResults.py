import sys


##Global Variables
f = open(sys.argv[1])
word = ""
count= 0
games = 0

#general values
fitness = 0.0
wins = 0
loss = 0

#score values
botScore = 0.0
enemyScore = 0.0

winBotUnitValue = 0.0
winBotConsValue = 0.0
winBotRescValue = 0.0
lossBotUnitValue = 0.0
lossBotConsValue = 0.0
lossBotRescValue = 0.0

winEnmUnitValue = 0.0
winEnmConsValue = 0.0
winEnmRescValue = 0.0
lossEnmUnitValue = 0.0
lossEnmConsValue = 0.0
lossEnmRescValue = 0.0

#time values
tempTime = 0
winTime = 0
lossTime = 0
fastWin = 10000000
fastLoss = 10000000
slowWin = 0
slowLoss = 0


for char in f.read():
    if (char == "\n"):  #end of line. Read all values of a single game
        #add to fitness holder

        enemyScore = enemyScore + float(word)
        if(winBool == 1):
            winEnmRescValue = winEnmRescValue + float(word)
        elif (winBool == 0):
            lossEnmRescValue = lossEnmRescValue + float(word)
        word = ""
        fitness = fitness + botScore / enemyScore 
        #reset temp values
        botScore = 0.0
        enemyScore = 0.0
        tempTime = 0
        count = 0
        games = games + 1
    else:
        if (char != ","): 
            word = word + char
        else: #end of word. Evaluate the read term
            count = count + 1 #words read +1

            if(count == 0 or count == 1 or count == 2 or count == 8): #default
                #do nothing
                word = ""
            elif (count == 3):  #time
                tempTime = tempTime + int(word)
            elif (count == 4):  #win/loss
                if (word == "win"):
                    wins = wins + 1
                    winTime = winTime + tempTime
                    if(tempTime > slowWin):
                        slowWin = tempTime
                    if(tempTime < fastWin):
                        fastWin = tempTime
                    winBool = 1
                elif (word == "loss"):
                    loss = loss + 1
                    lossTime = lossTime + tempTime
                    if(tempTime > slowLoss):
                        slowLoss = tempTime
                    if(tempTime < fastLoss):
                        fastLoss = tempTime
                    winBool = 0
                elif (word == "draw"):
                    winBool = -1
            elif (count == 5):  #bot Unit Score
                botScore = botScore + float(word)
                if(winBool == 1):
                    winBotUnitValue = winBotUnitValue + float(word)
                elif (winBool == 0):
                    lossBotUnitValue = lossBotUnitValue + float(word)
            elif (count == 6):  #bot Construction Score
                botScore = botScore + float(word)
                if(winBool == 1):
                    winBotConsValue = winBotConsValue + float(word)
                elif (winBool == 0):
                    lossBotConsValue = lossBotConsValue + float(word)
            elif (count == 7):  #bot Resource Score
                botScore = botScore + float(word)
                if(winBool == 1):
                    winBotRescValue = winBotRescValue + float(word)
                elif (winBool == 0):
                    lossBotRescValue = lossBotRescValue + float(word)
            elif (count == 9):  #enemy Unit Score
                enemyScore = enemyScore + float(word)
                if(winBool == 1):
                    winEnmUnitValue = winEnmUnitValue + float(word)
                elif (winBool == 0):
                    lossEnmUnitValue = lossEnmUnitValue + float(word)
            elif (count == 10): #enemy Construction Score
                enemyScore = enemyScore + float(word)
                if(winBool == 1):
                    winEnmConsValue = winEnmConsValue + float(word)
                elif (winBool == 0):
                    lossEnmConsValue = lossEnmConsValue + float(word)
            word = ""


print "\nGeneral Values"
print "\t number of games: %d" % games
print "\t wins:   %d" % wins
print "\t losses: %d" % loss
print "\t avgFitness: %f" % (fitness / games)

print "\nTime Values"
if (wins > 0) :
    print "\t avgWinTime : %d" % (winTime / wins)

if (loss > 0) :
    print "\t avgLossTime: %d\n" % (lossTime / loss)
print "\t fastest win : %d" % fastWin
print "\t fastest loss: %d" % fastLoss
print "\t slowest win : %d" % slowWin
print "\t slowest loss: %d" % slowLoss

print "\nScore Values"
if (wins>0):
    print "\t Wins:"
    print "\t    avgBotUnitScore: %f" % (winBotUnitValue / wins)
    print "\t    avgBotConsScore: %f" % (winBotConsValue / wins)
    print "\t    avgBotRescScore: %f" % (winBotRescValue / wins)
    print "\n\t    avgEnmUnitScore: %f" % (winEnmUnitValue / wins)
    print "\t    avgEnmConsScore: %f" % (winEnmConsValue / wins)
    print "\t    avgEnmRescScore: %f" % (winEnmRescValue / wins)
if (loss>0):
    print "\n\t Losses:"
    print "\t    avgBotUnitScore: %f" % (lossBotUnitValue / loss)
    print "\t    avgBotConsScore: %f" % (lossBotConsValue / loss)
    print "\t    avgBotRescScore: %f" % (lossBotRescValue / loss)
    print "\n\t    avgEnmUnitScore: %f" % (lossEnmUnitValue / loss)
    print "\t    avgEnmConsScore: %f" % (lossEnmConsValue / loss)
    print "\t    avgEnmRescScore: %f" % (lossEnmRescValue / loss)
print "\n\t Overall:"
print "\t    avgBotUnitScore: %f" % ((winBotUnitValue + lossBotUnitValue) / (wins + loss))
print "\t    avgBotConsScore: %f" % ((winBotConsValue + lossBotConsValue) / (wins + loss))
print "\t    avgBotRescScore: %f" % ((winBotRescValue + lossBotRescValue) / (wins + loss))
print "\n\t    avgEnmUnitScore: %f" % ((winEnmUnitValue + lossEnmUnitValue) / (wins + loss))
print "\t    avgEnmConsScore: %f" % ((winEnmConsValue + lossEnmConsValue) / (wins + loss))
print "\t    avgEnmRescScore: %f" % ((winEnmRescValue + lossEnmRescValue) / (wins + loss))


print "The following is a tab-spaced, spreadsheet-friendly output of the fields (scores are printed first for wins, then for losses):"
print "wins,losses,%wins,%losses,avgFitness,avgWinTime,avgLossTime,avgBotUnitScore,avgBotConsScore,avgBotResScore,avgEnmUnitScore,avgEnmConsScore,avgEnmRescScore"
print ("%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f" %
    (
        wins, loss, (float(wins)/(wins+loss)), (float(loss)/(wins+loss)),
        (fitness / games), (winTime / wins), (lossTime / loss), (winBotUnitValue / wins),
        (winBotConsValue / wins), (winBotRescValue / wins), (winEnmUnitValue / wins),
        (winEnmConsValue / wins), (winEnmRescValue / wins),
        (lossBotUnitValue / loss), (lossBotConsValue / loss), (lossBotRescValue / loss),
        (lossEnmUnitValue / loss), (lossEnmConsValue / loss), (lossEnmRescValue / loss)
    )).replace('.', ',') #spreadsheet is using comma as decimal separator...
