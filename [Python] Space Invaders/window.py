import pygame
from graphicUtils import loadImage, resizeImage, renderString

_RESIZE_BG = 'always' # {'never', 'once', 'always'}

class Window:
    #_window=None
    #width, height
    # background, icon

    def __init__(self, *, width, height, iconName="space-invaders", background="starry_bg", caption="PySpace Invaders" ):
        self._window = pygame.display.set_mode( (width, height), flags=pygame.RESIZABLE )
        #Title and Icon
        pygame.display.set_caption(caption)
        self._icon = loadImage(iconName)
        self.background = loadImage(background)
        if _RESIZE_BG != 'never':
            self.background = resizeImage(self.background, self.getWindowSize())
        pygame.display.set_icon(self._icon)

    def getWidth(self):
        return self._window.get_width()
    def getHeight(self):
        return self._window.get_height()
    def getWindowSize(self):
        return self._window.get_width(), self._window.get_height()

    def resized(self, newW, newH):
        if _RESIZE_BG=='always':
            self.background = resizeImage(self.background, (newW, newH))

    def write(self, string, pixX, pixY, color=(255, 255, 255)):
        rendered = renderString(string, color)
        self._window.blit(rendered, (pixX, pixY))

    def drawUnit(self, unit):
        image = resizeImage(
            unit.image,
            self.toPixelDimensions(unit.simulation, unit.width, unit.height)
        )
        win_coord = self.toWindowCoordinates(unit.simulation, *unit.getCoord())
        win_coord = (
            win_coord[0]-image.get_width()/2,
            win_coord[1]-image.get_height()/2
        )
        #print(type(unit), unit.getCoord(), win_coord)
        self._window.blit(image, win_coord)

    def draw(self, image, coord):
        self._window.blit(image, coord)

    def clear(self):
        self.fill( (0, 0, 20) )
        self.draw(self.background, (0, 0))

    def fill(self, fill):
        self._window.fill(fill)

    def toSimulationCoordinates(self, simulation, pixX, pixY):
        return simulation.toSimulationCoordinates(self, pixX, pixY)

    def toWindowCoordinates(self, simulation, simX, simY):
        return simulation.toWindowCoordinates(self, simX, simY)

    def toSimulationDimensions(self, simulation, pix_width, pix_height):
        sim_w = ( simulation._width*pix_width )/self.getWidth()
        sim_h = ( simulation._height*pix_height )/self.getHeight()
        return sim_w, sim_h

    def toPixelDimensions(self, simulation, sim_width, sim_height):
        pix_width = ( self.getWidth()*sim_width )/ simulation._width
        pix_height = ( self.getHeight()*sim_height )/ simulation._height
        return pix_width, pix_height