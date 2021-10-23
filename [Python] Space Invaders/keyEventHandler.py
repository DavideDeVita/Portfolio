import pygame

keydownEvtKeyHandler = {
    pygame.K_a: lambda player: player.updateDirection( x=-1 ),
    pygame.K_LEFT: lambda player: player.updateDirection( x=-1 ),
    pygame.K_d: lambda player: player.updateDirection( x=1 ),
    pygame.K_RIGHT: lambda player: player.updateDirection( x=1 ),
    pygame.K_w: lambda player: player.updateDirection( y=1 ),
    pygame.K_UP: lambda player: player.updateDirection( y=1 ),
    pygame.K_s: lambda player: player.updateDirection( y=-1 ),
    pygame.K_DOWN: lambda player: player.updateDirection( y=-1 ),
    pygame.K_SPACE: lambda player: player.fireBullet( ),
}

keyupEvtKeyHandler = {
    pygame.K_a: lambda player: player.updateDirection( x=1 ),
    pygame.K_LEFT: lambda player: player.updateDirection( x=1 ),
    pygame.K_d: lambda player: player.updateDirection( x=-1 ),
    pygame.K_RIGHT: lambda player: player.updateDirection( x=-1 ),
    pygame.K_w: lambda player: player.updateDirection( y=-1 ),
    pygame.K_UP: lambda player: player.updateDirection( y=-1 ),
    pygame.K_s: lambda player: player.updateDirection( y=1 ),
    pygame.K_DOWN: lambda player: player.updateDirection( y=1 ),
}

def handleKeydownEvent(key):
    return keydownEvtKeyHandler.get(key, lambda p: None)

def handleKeyupEvent(key):
    return keyupEvtKeyHandler.get(key, lambda p: None)

def handlePressedKeys(player):
    keys = pygame.key.get_pressed()
    if keys[pygame.K_a] or keys[pygame.K_LEFT]:
        player.updateDirection((-1, 0))
    if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
        player.updateDirection((1, 0))
    if keys[pygame.K_w] or keys[pygame.K_UP]:
        player.updateDirection((0, -1))
    if keys[pygame.K_s] or keys[pygame.K_DOWN]:
        player.updateDirection((0, 1))