import sys
import re
from ambuexcept import *

person = [None]
nambulance = [None]
hospital = [None]
actionseq = [None]

def manhattanDistance(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

def usage():
    print ("python val.py inputFile outputFile")
    exit(2)

def loadInput(infile):
    global person, nambulance
    with open(infile) as f:
        # People section
        for line in f:
            if line[0] == 'p':
                continue
            if not line.strip():
                break
            person.append(tuple([i for i in map(int, line.split(','))]))
        # Numambulance section
        for line in f:
            if line[0] == 'h' or not line.strip():
                continue
            nambulance.append(int(line))

def loadSolution(outfile):
    def parseAction(s):
        return s[0], int(s[1:])

    global hospital, actionseq
    lp = re.compile(r'\s*\([^)]*\)\s*') # used filter out everything in ()
    with open(outfile) as f:
        # Hospital section
        for line in f:
            line = line.upper()
            if line[0] == 'H':
                continue
            if not line.strip():
                break
            line = line.split(':')[1].strip(' ()\n\r')
            hospital.append(tuple([i for i in map(int, line.split(','))]))
        # Ambulance section
        for line in f:
            line = line.upper()
            if line[0] == 'A' or not line.strip():
                continue
            line = line.split(':')[1].strip()
            actionseq.append([i for i in map(parseAction, lp.split(line)[:-1])])

def validate(ambunum, actions):
    def unload(ambunum, hnum, currentTime, lastStop, onload):
        global saved
        currentStop = hospital[hnum]
        currentTime += 1 + manhattanDistance(currentStop, lastStop)
        for p in onload:
            if person[p][2] < currentTime:
                raise PeopleDieException(ambunum, p, person[p],
                                         hnum, currentTime)
            if saved[p]:
                raise SaveTwiceException(ambunum, p,
                                         hnum, saved[p],
                                         currentTime)
            saved[p] = (ambunum, hnum, currentTime)
        return currentTime, currentStop
    
    def load(ambunum, pnum, currentTime, lastStop, onload):
        currentStop = person[pnum]
        currentTime += 1 + manhattanDistance(currentStop, lastStop)
        onload.append(pnum)
        if len(onload) > 4:
            raise OverloadException(ambunum, onload, currentTime)
        return currentTime, currentStop

    currentTime = 0
    onload = []

    if actions[0][0] != 'H':
        raise StartHospitalException(ambunum)
    else:
        hnum = actions[0][1]
        nambulance[hnum] -= 1
        if nambulance[hnum] < 0:
            raise MoreAmbulanceException(ambunum, hnum)

    lastStop = hospital[actions[0][1]]
    for action in actions[1:]:
        if action[0] == 'H':
            currentTime, lastStop = \
                         unload(ambunum, action[1], currentTime,
                                lastStop, onload)
            onload = []
        elif action[0] == 'P':
            currentTime, lastStop = \
                         load(ambunum, action[1], currentTime,
                              lastStop, onload)
        else:
            raise FormatException(ambunum, action)

    if actions[-1][0] != 'H':
        raise UnloadHospitalException(ambunum)

def generate_html():
    pass

if __name__ == '__main__':
    if len(sys.argv) < 3:
        usage()

    loadInput(sys.argv[1])
    loadSolution(sys.argv[2])
    saved = [False] * len(person)
    try:
        for ambunum, actions in enumerate(actionseq[1:], start = 1):
            validate(ambunum, actions)
    except AmbulanceException as e:
        print (e.msg())
    finally:
        print ('Total saved: {}.'.format(sum(i != False for i in saved)))
        generate_html()
