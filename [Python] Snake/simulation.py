from window import Window
from random import randint
from snake import Snake

_x=0
_y=1
pelletScore = 100

class Simulation:
    #level
    #units

    def __init__(self, rows:int, cols:int):
        self.cols = cols
        self.rows = rows
        x, y = int(cols/2), int(rows/2)
        self.snake = Snake(self, positions=[(x, y), (x-1, y)], direction='right')
        self.placePelletRandomly()
        self.youLose=False
        self.score=0

    def gameOver(self) -> bool :
        return self.youLose#self.player is None
    def youWin(self) -> bool :
        return self.snake.len() == self.cols * self.rows

    def getScore(self) -> int :
        return self.score*pelletScore

    def update(self):
        newPos = self.snake.update()
        if newPos==self.pellet:
            self.score+=1
            self.snake.grow()
            self.placePelletRandomly()
        else:
            self.youLose = self.snake.checkGameOver()

    def render(self, window:Window):
        window.drawPellet(self.pellet)
        self.snake.render(window)

    def placePelletRandomly(self):
        while True:
            x, y= randint(0, self.cols-1), randint(0, self.rows-1)
            if not self.snakeIn(x, y):
                self.pellet = (x, y)
                break

    def snakeIn(self, x:int, y:int) -> bool :
        return self.snake.isIn(x, y)

    #sim x,y are coordinates in the map
    def toWindowCoordinates(self, window:Window, sim_x:int, sim_y:int) -> (int, int) :
        win_x = (window.getWidth() / self.cols) * sim_x
        win_y = (window.getHeight() / self.rows) * sim_y
        return win_x, win_y

    def toSimulationCoordinates(self, window:Window, pix_x:int, pix_y:int) -> (int, int) :
        sim_x = pix_x * (self.cols / window.getWidth())
        sim_y = pix_y * (self.rows / window.getHeight())
        return sim_x, sim_y

    def toSimulationDimensions(self, window:Window, pix_width:int, pix_height:int) -> (int, int) :
        return window.toSimulationDimensions(self, pix_width, pix_height)

    def toPixelDimensions(self, window:Window, sim_width:int, sim_height:int) -> (int, int) :
        return window.toPixelDimensions(self, sim_width, sim_height)