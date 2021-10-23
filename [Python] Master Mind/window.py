import pygame
from pygame import Rect
from graphicUtils import loadImage, resizeImage, renderString

_RESIZE_BG = 'always' # {'never', 'once', 'always'}

class Window:
    #_window=None
    #width, height
    # background, icon

    def __init__(self, *, width, height, iconName=None, background=None, caption):
        self._window = pygame.display.set_mode( (width, height), flags=pygame.RESIZABLE )
        #Title and Icon
        pygame.display.set_caption(caption)
        if iconName:
            self._icon = loadImage(iconName)
            pygame.display.set_icon(self._icon)
        if background:
            self.background = loadImage(background)
            if _RESIZE_BG != 'never':
                self.background = resizeImage(self.background, self.getWindowSize())
        else: self.background=None

    def getWidth(self):
        return self._window.get_width()
    def getHeight(self):
        return self._window.get_height()
    def getWindowSize(self):
        return self._window.get_width(), self._window.get_height()

    def resized(self, newW, newH):
        if _RESIZE_BG=='always' and self.background:
            self.background = resizeImage(self.background, (newW, newH))

    def write(self, fontName, string, pixX, pixY, color=(255, 255, 255)):
        rendered = renderString(fontName, string, color)
        self._window.blit(rendered, (pixX, pixY))

    def draw(self, image, coord):
        self._window.blit(image, coord)

    def clear(self):
        self.fill( (0, 0, 0) )
        if self.background:
            self.draw(self.background, (0, 0))

    def fill(self, fill):
        self._window.fill(fill)