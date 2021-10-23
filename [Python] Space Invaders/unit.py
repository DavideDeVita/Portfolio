import graphicUtils as gUt

class Unit:
    # simulation
    # coord = {'x':x, 'y':y}
    # width, halfWidth
    # height, halfHeight
    # image
    # speed

    def __init__(self, x, y, simulation, *, side, width=1, height=1, speed=1, direction=None, type, name):
        self.name = name
        self.simulation = simulation
        self.coord = {'x':x, 'y':y}
        self.width = width
        self.halfWidth = width/2
        self.height = height
        self.halfHeight = height/2
        self.side = side
        self.image = gUt.loadImage( type, keep=True )
        self.speed = speed
        self.direction = direction if direction is not None else {'x':0, 'y':0}

    def __str__(self):
        return self.name

    #old
    def move(self, timeElapsed, dir=None):
        if dir is None:
            dir = self.direction
        self.coord['x'] += self.speed * dir['x'] * timeElapsed
        self.coord['y'] += self.speed * dir['y'] * timeElapsed

        self.simulation.checkBoundaries(self)

    def getCoord(self):
        return self.coord['x'], self.coord['y']

    def setCoordinates(self, *, x=None, y=None):
        if x is not None:
            self.coord['x']=x
        if y is not None:
            self.coord['y']=y

    def setDirection(self, *, x=None, y=None):
        if x is not None:
            self.direction['x']=x
        if y is not None:
            self.direction['y']=y

    def updateDirection(self, *, x=None, y=None):
        if x is not None:
            self.direction['x']+=x
        if y is not None:
            self.direction['y']+=y

    def update(self, timeElapsed=1):
        self.move(timeElapsed, self.direction)

    def dispose(self):
        self.simulation.disposeUnit(self)
        #print(self, "is disposed")

    def collision(self):
        self.dispose()

    def fireBullet(self, direction={'x':0, 'y':1}):
        self.simulation.createBullet(self, direction=direction, side=self.side)

    def asRect(self):
        return self.simulation.rectOf(self)