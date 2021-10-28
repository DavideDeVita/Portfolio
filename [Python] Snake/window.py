import pygame
from graphicUtils import loadImage, resizeImage, renderString

_TILE_DIM=1
_PELLET_DIM=0.8
_SNAKE_DIM=1

_RESIZE_BG = 'always' # {'never', 'once', 'always'}

class Window:
    #_window=None
    #width, height
    # background

    def __init__(self, *, width, height, simulation, iconName=None, background=None, caption="Koka Snake" ):
        self._window = pygame.display.set_mode( (width, height), flags=pygame.RESIZABLE )
        self.simulation = simulation
        #Title and Icon
        pygame.display.set_caption(caption)
        if iconName is not None:
            pygame.display.set_icon( loadImage(iconName) )

        if background is not None:
            self.background = loadImage(background)
            if _RESIZE_BG != 'never':
                self.background = resizeImage(self.background, self.getWindowSize())
        else:
            self.background = None

    def getWidth(self):
        return self._window.get_width()
    def getHeight(self):
        return self._window.get_height()
    def getWindowSize(self):
        return self._window.get_width(), self._window.get_height()

    def resized(self, newW, newH):
        if _RESIZE_BG=='always' and self.background is not None:
            self.background = resizeImage(self.background, (newW, newH))

    def write(self, string, pixX, pixY, color=(255, 255, 255)):
        rendered = renderString(string, color)
        self._window.blit(rendered, (pixX, pixY))

    def drawPellet(self, pellet:(int, int)):
        image = loadImage('pellet', keep=True)
        image = resizeImage(
            image,
            self.toPixelDimensions(self.simulation, _PELLET_DIM, _PELLET_DIM)
        )
        win_coord = self.toWindowCoordinates(self.simulation, *pellet)
        win_coord = (
                win_coord[0],
                win_coord[1]
        )
        #print(type(unit), unit.getCoord(), win_coord)
        self._window.blit(image, win_coord)

    def drawBody(self, body:(int, int), dir:str):
        if dir is not None:
            image = loadImage('snakebody_'+dir, keep=True)
            image = resizeImage(
                image,
                self.toPixelDimensions(self.simulation, _SNAKE_DIM, _SNAKE_DIM)
            )
            win_coord = self.toWindowCoordinates(self.simulation, *body)
            win_coord = (
                win_coord[0],
                win_coord[1]
            )
            #print(type(unit), unit.getCoord(), win_coord)
            self._window.blit(image, win_coord)

    def drawHead(self, head:(int, int), dir:str):
        if dir is not None:
            image = loadImage('snakehead_'+dir, keep=True)
            image = resizeImage(
                image,
                self.toPixelDimensions(self.simulation, _SNAKE_DIM, _SNAKE_DIM)
            )
            win_coord = self.toWindowCoordinates(self.simulation, *head)
            win_coord = (
                win_coord[0],
                win_coord[1]
            )
            #print(type(unit), unit.getCoord(), win_coord)
            self._window.blit(image, win_coord)

    def draw(self, image, coord):
        self._window.blit(image, coord)

    def clear(self):
        self.fill( (0, 0, 0) )
        if self.background is not None:
            self.draw(self.background, (0, 0))

    def fill(self, fill):
        self._window.fill(fill)

    def toSimulationCoordinates(self, simulation, pixX, pixY):
        return simulation.toSimulationCoordinates(self, pixX, pixY)

    def toWindowCoordinates(self, simulation, simX, simY):
        return simulation.toWindowCoordinates(self, simX, simY)

    def toSimulationDimensions(self, simulation, pix_width, pix_height):
        sim_w = (simulation.cols * pix_width) / self.getWidth()
        sim_h = (simulation.rows * pix_height) / self.getHeight()
        return sim_w, sim_h

    def toPixelDimensions(self, simulation, sim_width, sim_height):
        pix_width = ( self.getWidth()*sim_width )/ simulation.cols
        pix_height = ( self.getHeight()*sim_height )/ simulation.rows
        return pix_width, pix_height