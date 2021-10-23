from window import Window
import graphicUtils as gUt
import pygame
import mastermindSequence as mmSeq

#Checks all the event that have occurred since the last loop
def evtCheck():
    global running
    for evt in pygame.event.get():
        if evt.type == pygame.MOUSEBUTTONUP:
            clicked = [c for c in clickables if c['rect'].collidepoint(pygame.mouse.get_pos())]
            if len(clicked)>0:
                clicked=clicked[0]
                if clicked['action'] =='guess':
                    guess()
                elif clicked['action'] =='select':
                    select(clicked['select'])
                else:
                    changeColor(clicked['color'])

        elif evt.type == pygame.VIDEORESIZE:
            print ("window resized", evt.w, evt.h)
            window.resized(evt.w, evt.h)
        elif evt.type == pygame.QUIT:
            running = False

def guess():
    global currentGuess, selected, gameWon, gameLost
    if None not in currentGuess:
        currentGuess = mmSeq.checkGuess(secretSequence, currentGuess)
        previousGuesses.append(currentGuess)
        if currentGuess[4] == 4:
            gameWon = True
        elif len(previousGuesses)==6:
            gameLost=True

        currentGuess =[None for i in range(4)]
        selected=0

def select(index):
    global selected
    selected=index

def changeColor(color):
    global currentGuess, selected
    if 0<selected<5:
        currentGuess = currentGuess[:selected-1]+[color]+currentGuess[selected:]

pygame.init()
gUt.initFont()
window = Window(width=1000, height=600, caption="py MasterMind", iconName='icon')

clickables = []
secretSequence = mmSeq.generateRandomSequence(gUt.selectableColors, 4)
previousGuesses = []
currentGuess = [None for i in range(4)]
selected = 0
gameWon = gameLost = False

running = True
while running:
    evtCheck()
    #Redraw
    clickables = gUt.drawMastermindBoard(window, previousGuesses=previousGuesses, currentGuess=currentGuess, selected=selected)

    if gameWon:
        gUt.gameWon(window)
    elif gameLost:
        gUt.gameLost(window, secretSequence)
        currentGuess = secretSequence

    gUt.updateView()

    if gameWon or gameLost:
        running=False
        pygame.time.wait(1500)