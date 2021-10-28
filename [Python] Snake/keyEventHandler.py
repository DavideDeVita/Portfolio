import fps
import pygame

keydownEvtKeyHandler = {
    pygame.K_a: lambda player: player.goLeft(),
    pygame.K_LEFT: lambda player: player.goLeft(),
    pygame.K_d: lambda player: player.goRight(),
    pygame.K_RIGHT: lambda player: player.goRight(),
    pygame.K_w: lambda player: player.goUp(),
    pygame.K_UP: lambda player: player.goUp(),
    pygame.K_s: lambda player: player.goDown(),
    pygame.K_DOWN: lambda player: player.goDown(),
    pygame.K_SPACE: lambda player: player.fireBullet( ),
}

def handleKeydownEvent(key):
    fps.forceFrame=True
    return keydownEvtKeyHandler.get(key, lambda p: None)