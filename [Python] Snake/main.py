import pygame
import graphicUtils as grUtil
import audio
import fps
import keyEventHandler as keh
from window import Window
from simulation import Simulation

#Checks all the event that have occurred since the last loop
def evtCheck():
    global running
    for evt in pygame.event.get():
        if evt.type==pygame.KEYDOWN:
            keh.handleKeydownEvent(evt.key)(simulation.snake)
        elif evt.type == pygame.VIDEORESIZE:
            print ("window resized", evt.w, evt.h)
            window.resized(evt.w, evt.h)
        elif evt.type == pygame.QUIT:
            running = False

# initialize
pygame.init()
grUtil.initFont()
##audio.playBackgroundMusic()
simulation = Simulation(15, 15)
window = Window(width=600, height=600, simulation=simulation, iconName='icon')

#simulation.createPlayerUnit(0, 2.5, speed=4.11)

# Game loop
running = True
while running:
    evtCheck()
    #FPS Check
    if not fps.forceFrame and not fps.isFPS_respected():
        continue
    timeElapsed = fps.logFrameTime()

    window.clear()

    simulation.update()

    ####simulation.handleCollisions()
    if simulation.gameOver():
        window.write("Game Over\nScore: "+str(simulation.getScore()), window.getWidth()/2 - 100, window.getHeight()/2 - 20)
        grUtil.updateView()
        #audio.playYouLose()
        pygame.time.wait(3000)
        break
    elif simulation.youWin():
        window.write("You Win\nScore: "+str(simulation.getScore()), window.getWidth()/2 - 100, window.getHeight()/2 - 20)
        grUtil.updateView()
        #audio.playYouWin()
        pygame.time.wait(3000)
        break

    simulation.render(window)

    #units update

    grUtil.updateView()

    fps.FPS = 8+ int(simulation.score/5)