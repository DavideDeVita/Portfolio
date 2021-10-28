import pygame
from random import choice
_ALLOW_SMOOTH_RESIZE=False

images = {
    # 'enemy': pygame.image.load("enemy.png"),
    # 'player': pygame.image.load("player.png"),
    # 'space-invaders': pygame.image.load("space-invaders.png")
}

def initFont():
    global font
    font = pygame.font.SysFont("inkfree", 64)

#loads an image
def loadImage(name, *, keep=False):
    if name not in images:
        image = pygame.image.load("res/images/"+name + ".png")
        if keep:
            images[name] = image
        return image
    else:
        return images[name]

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