from window import Window
import graphicUtils as gUt
import pygame
import mastermindSequence as mmSeq

#Checks all the event that have occurred since the last loop
def evtCheck():
    global running, renderAll
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
            renderAll=True
        elif evt.type == pygame.QUIT:
            running = False

def guess():
    global currentGuess, selected, gameWon, gameLost, renderLeft, renderGuess
    if None not in currentGuess:
        currentGuess = mmSeq.checkGuess(secretSequence, currentGuess)
        previousGuesses.append(currentGuess)
        if currentGuess[4] == 4:
            gameWon = True
        elif len(previousGuesses)==6:
            gameLost=True

        currentGuess =[None for i in range(4)]
        selected=0
        renderLeft=True
        renderGuess=True

def select(index):
    global selected,renderGuess
    selected=index
    renderGuess=True

def changeColor(color):
    global currentGuess, selected, renderGuess
    if 0<selected<5:
        currentGuess = currentGuess[:selected-1]+[color]+currentGuess[selected:]
        renderGuess=True

pygame.init()
gUt.initFont()
window = Window(width=1000, height=600, caption="py MasterMind", iconName='icon')

clickables = []
secretSequence = mmSeq.generateRandomSequence(gUt.selectableColors, 4)
previousGuesses = []
currentGuess = [None for i in range(4)]
selected = 0
gameWon = gameLost = False
renderLeft,renderGuess, renderAll = True, True, True

running = True
while running:
    evtCheck()
    #Redraw
    #print("left", renderLeft, "guess", renderGuess, "all", renderAll)
    if renderLeft or renderGuess or renderAll:
        clickables = gUt.drawMastermindBoard(window, previousGuesses=previousGuesses, currentGuess=currentGuess,
                                 selected=selected, renderLeft=renderLeft, renderGuess=renderGuess, renderAll=renderAll)
        renderLeft, renderGuess, renderAll = False, False, False

    if gameWon:
        gUt.gameWon(window)
    elif gameLost:
        gUt.gameLost(window, secretSequence)
        #currentGuess = secretSequence

    gUt.updateView()

    if gameWon or gameLost:
        running=False
        pygame.time.wait(6090)