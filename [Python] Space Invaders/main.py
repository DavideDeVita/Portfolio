import pygame
import graphicUtils as grUtil
import audio
import random
import fps
import keyEventHandler as keh
from window import Window
from simulation import Simulation

#Checks all the event that have occurred since the last loop
def evtCheck():
    global running
    for evt in pygame.event.get():
        if evt.type==pygame.KEYDOWN:
            keh.handleKeydownEvent(evt.key)(simulation.player)
        elif evt.type==pygame.KEYUP:
            keh.handleKeyupEvent(evt.key)(simulation.player)
        elif evt.type == pygame.VIDEORESIZE:
            print ("window resized", evt.w, evt.h)
            window.resized(evt.w, evt.h)
        elif evt.type == pygame.QUIT:
            running = False

    #handlePressedKeys(player1)

# initialize
pygame.init()
grUtil.initFont()
audio.playBackgroundMusic()
window = Window(width=1000, height=600)
simulation = Simulation(xMin=-15, yMin=0, xMax=15, yMax=(600/1000)*(30-0))

simulation.createPlayerUnit(0, 2.5, speed=4.11)

for y in range(170, 80, -15):
    r = random.randint(-2, 10)*10
    for x in range(-110+r, 40+r, 17):
        simulation.createEnemyUnit(x/10., y/10., speed=9.6, width=0.75, height=0.75)

print(fps.logFrameTime())

# Game loop
running = True
while running:
    evtCheck()
    #FPS Check
    if not fps.isFPS_respected():
        continue
    timeElapsed = fps.logFrameTime()

    window.clear()

    simulation.handleCollisions()
    if simulation.gameOver():
        window.write("Game Over :(", 400, 270)
        grUtil.updateView()
        audio.playYouLose()
        pygame.time.wait(3000)
        break
    elif simulation.youWin():
        window.write("You Win", 400, 270)
        grUtil.updateView()
        audio.playYouWin()
        pygame.time.wait(3000)
        break

    #The original Space Invaders game updated only 1 enemy per frame
    for ally in simulation.getAllies():
        ally.update(timeElapsed)

    enemy=simulation.fetchEnemy()
    if enemy:
        enemy.update(timeElapsed)

    for u in simulation.getUnits():
        #u.update()
        window.drawUnit(u)
    grUtil.updateView()