import window as win

directionByName = {
    'up':(0,-1),
    'down':(0,1),
    'right':(1,0),
    'left':(-1,0)
}
opposites = {
    'up':'down',
    'down':'up',
    'right':'left',
    'left':'right',
    None:None
}

def oppositeDir(dir:str) -> str:
    return opposites[dir]


class Snake:
    snake=[] # (x, y)
    directions = []
    appendNext=False

    def __init__(self, simulation, positions:list, direction:str):
        self.snake = positions[:]
        self.currDir = direction
        self.directions = [direction]
        self.simulation=simulation

    def update(self):
        from simulation import _x, _y
        dir = directionByName[self.currDir]
        head = self.snake[0]
        new = (head[_x] + dir[_x])%self.simulation.cols, \
              (head[_y] + dir[_y])%self.simulation.rows
        if not self.appendNext:
            self.snake = [new] + self.snake[:-1]
            self.directions = [self.currDir] + self.directions[:-1]
        else:
            self.snake = [new] + self.snake[:]
            self.directions = [self.currDir] + self.directions[:]
            self.appendNext=False
        return new

    def render(self, window:win.Window):
        d_i=-1
        dir = self.directions[-1]
        #Post of Tail (only post)
        window.drawBody(self.snake[-1], dir)
        for body in self.snake[-2:0:-1]:
            #Pre
            window.drawBody(body, oppositeDir(dir) )
            d_i-=1
            dir = self.directions[d_i]
            window.drawBody(body, dir)
        window.drawHead(self.snake[0], dir )

    def grow(self):
        self.appendNext=True

    def len(self) -> int:
        return len(self.snake)

    def isIn(self, x: int, y: int) -> bool:
        return (x, y) in self.snake

    def checkGameOver(self) -> bool :
        return self.snake[0] in self.snake[1:]

    def goUp(self):
        if self.currDir!='down':
            self.currDir='up'
    def goLeft(self):
        if self.currDir!='right':
            self.currDir='left'
    def goDown(self):
        if self.currDir!='up':
            self.currDir='down'
    def goRight(self):
        if self.currDir!='left':
            self.currDir='right'