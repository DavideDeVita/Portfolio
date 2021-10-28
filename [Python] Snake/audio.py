from pygame import mixer

sounds = {}

def playBackgroundMusic():
    mixer.music.load('res/audio/background.wav')
    mixer.music.play(-1)

def playSound(soundName):
    if soundName not in sounds:
        sounds[soundName] = mixer.Sound('res/audio/'+soundName+'.wav')
    sounds[soundName].play()

def playYouWin():
    mixer.Sound('res/audio/youWin.wav').play()

def playYouLose():
    mixer.Sound('res/audio/gameOver.wav').play()