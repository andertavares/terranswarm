import os
import zipfile
import sys
import shutil
import xml.etree.ElementTree as ET


def dateDif (stringstart, stringend):

    starttemp = stringstart.split('.')
    startdate = starttemp[0].split('-')
    starttime = starttemp[1].split(':')
    
    endtemp = stringend.split('.')
    enddate = endtemp[0].split('-')
    endtime = endtemp[1].split(':')


    carry_yr  = 12*(int(enddate[0]) - int(startdate[0]))

    #bisextual year
    yr4div = (float(enddate[0]) / 4.0) == 0.0
    yr100div = (float(enddate[0]) / 100.0) == 0.0
    yr400div = (float(enddate[0]) / 400.0) == 0.0

    if(enddate[1]=='02' and (yr4div and (not yr100div or yr400div))):
        carry_mth = 29*(carry_yr + int(enddate[1]) - int(startdate[1]))
    elif(enddate[1]=='02'):
        carry_mth = 28*(carry_yr + int(enddate[1]) - int(startdate[1]))
    elif(enddate[1] == '01' or enddate[1] == '03' or enddate[1] == '05' or enddate[1] == '07' or enddate[1] == '08' or enddate[1] == '10' or enddate[1] == '12'):
        carry_mth = 31*(carry_yr + int(enddate[1]) - int(startdate[1]))
    else:
        carry_mth = 30*(carry_yr + int(enddate[1]) - int(startdate[1]))

    carry_day = 24*(carry_mth + int(enddate[2]) - int(startdate[2]))

    carry_hr  = 60*(carry_day + int(endtime[0]) - int(starttime[0])) 
    carry_min = 60*(carry_hr + int(endtime[1]) - int(starttime[1]))
    carry_sec = (carry_min + int(endtime[2]) - int(starttime[2]))

#    print carry_yr
#    print carry_mth
#    print carry_day
#    print carry_hr
#    print carry_min
#    print carry_sec

    return carry_sec


if __name__ == '__main__':

    filePath = sys.argv[1]

    archive = zipfile.ZipFile(filePath, 'r')
    fileList = archive.namelist()
    exectime=0

    for f in fileList:
        if (f.split('.')[-1] == 'xml'):
            xml_tree = ET.parse(archive.open(f)).getroot()
            stringstart = xml_tree.find('start').get('value')
            stringend = xml_tree.find('end').get('value')


            dif = dateDif (stringstart, stringend)
            if (dif < 0):
                print 'Negative difference(%d) in file %s!' %(dif,f)
                exit()
            exectime = exectime + dif
           
    print exectime
