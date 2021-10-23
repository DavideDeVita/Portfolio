import pygame
from random import choice
_x=0
_y=1
_ALLOW_SMOOTH_RESIZE=False

icons = {
    # 'enemy': pygame.image.load("enemy.png"),
    # 'player': pygame.image.load("player.png"),
    # 'space-invaders': pygame.image.load("space-invaders.png")
}
bullets = []

def initFont():
    global font
    font = pygame.font.SysFont("inkfree", 64)

#loads an image
def loadImage(name, *, keep=False):
    if name=='bullet':
        return bulletImage()
    #else
    if name not in icons:
        image = pygame.image.load("res/images/"+name + ".png")
        if keep:
            icons[name] = image
        return image
    else:
        return icons[name]

def bulletImage():
    if len(bullets)==0:
        for i in range(1,9):
            bullets.append(loadImage("bullets/bullet"+str(i)))
    return choice(bullets)

#loads an image
def resizeImage(img, newSize):
    if(_ALLOW_SMOOTH_RESIZE):
        return pygame.transform.smoothscale(img, newSize)
    else:
        return pygame.transform.scale(img, newSize)

#updates the screen display in the game loop
def updateView():
    pygame.display.update()

def renderString(string, color):
    return font.render(string, True, color)