FPS=60.
lastTimeStamp=0

def getTimestamp():
    import pygame.time as pyt
    return pyt.get_ticks()

def logFrameTime():
    global lastTimeStamp
    ret = sinceLast()
    lastTimeStamp = getTimestamp()
    return ret

def isFPS_respected():
    return getDurationS(lastTimeStamp, getTimestamp())>=1./FPS

def getDurationS(t1, t2):
    if t1<t2:
        return (t2-t1)/1000.
    else:
        return (t1-t2)/1000.

def sinceLast():
    return getDurationS(lastTimeStamp, getTimestamp())