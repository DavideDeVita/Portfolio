import unit as u
import selfMovingUnit as smu
import audio as aud

class Simulation:
    _bounds = {}
    allies = []
    enemies = []
    player=None
    #_width, _height
    #zones = []  #Was meant to be used for collision, but it it unnecessarily complicated

    def __init__(self, *, xMin=0, xMax, yMin=0, yMax):
        self._bounds["xMin"]=xMin
        self._bounds["xMax"]=xMax
        self._bounds["yMin"]=yMin
        self._bounds["yMax"]=yMax
        self._width = xMax-xMin
        self._height = yMax-yMin

    #units utilites
    def getUnits(self):
        return self.allies+self.enemies
    def getAllies(self):
        return self.allies[:]
    def getEnemies(self):
        return self.enemies[:]
    def fetchEnemy(self):
        if len(self.enemies)>0:
            self.enemies = self.enemies[-1:]+self.enemies[:-1]
            return self.enemies[0]
        return None

    def gameOver(self):
        return self.player is None
    def youWin(self):
        return len(self.enemies)==0

    def addUnit(self, unit):
        if unit.side=='allies':
            self.allies.append(unit)
        else:
            self.enemies.append(unit)

    def createPlayerUnit(self, x, y, **others):
        self.player = u.Unit(x, y, self, side='allies', type='player', name='Player', **others)
        self.addUnit( self.player )

    def createSelfMovingUnit(self, x, y, *, side, name, **others):
        newUnit = smu.SelfMovingUnit(x, y, self, side=side, name=name, **others)
        self.addUnit( newUnit )

    def createEnemyUnit(self, x, y, **others):
        others['type'] = 'enemy'
        if 'direction' not in others:   others['direction'] = {'x':1, 'y':0}
        if 'onXBound' not in others:    others['onXBound'] = smu.STD_ENEMY_XBOUND_EVT
        if 'onYBound' not in others:    others['onYBound'] = smu.STD_ENEMY_YBOUND_EVT
        newUnit = smu.SelfMovingUnit(x, y, self, side='enemies', name='enemy', **others)
        self.addUnit( newUnit )

    def createBullet(self, unit, **others):
        others['type'] = 'bullet'
        if 'width' not in others:   others['width'] = 0.66
        if 'height' not in others:   others['height'] = 0.66
        if 'direction' not in others:   others['direction'] = {'x':0, 'y':1}
        if 'speed' not in others:       others['speed'] = 11.4
        if 'onYBound' not in others:    others['onYBound'] = smu.STD_BULLET_YBOUND_EVT
        x = unit.coord['x'] + others['direction']['x']
        y = unit.coord['y'] + others['direction']['y']
        newUnit = smu.SelfMovingUnit(x, y, self, name='bullet', **others)
        aud.playSound('laser')
        self.addUnit( newUnit )

    def disposeUnit(self, unit):
        if unit.side=='allies':
            self.allies.remove(unit)
            if self.player==unit:
                self.player=None
        else:
            self.enemies.remove(unit)

    def rectOf(self, unit):
        return Rect(unit.coord['x']-unit.halfWidth, unit.coord['y']+unit.halfHeight, unit.width, unit.height)

    def handleCollisions(self):
        collided = set()
        for ally in self.allies:
            for enemy in self.enemies:
                if ally.asRect().collidesWith( enemy.asRect() ):
                    #collisions.union( {ally, enemy} )
                    collided.add( ally )
                    collided.add( enemy )
                    #print("Collision detected between ",ally, "and", enemy)
                    aud.playSound('explosion')
        for unit in collided:
            unit.collision()

    # Utilities
    def checkBoundaries(self, unit):
        if unit.coord['x']-unit.halfWidth < self._bounds["xMin"]:
            unit.coord['x'] = self._bounds["xMin"]+unit.halfWidth
        elif unit.coord['x']+unit.halfWidth > self._bounds["xMax"]:
            unit.coord['x'] = self._bounds["xMax"]-unit.halfWidth

        if unit.coord['y'] - unit.halfHeight < self._bounds["yMin"]:
            unit.coord['y'] = self._bounds["yMin"]+unit.halfHeight
        elif unit.coord['y'] + unit.halfHeight > self._bounds["yMax"]:
            unit.coord['y'] = self._bounds["yMax"]-unit.halfHeight

    def toWindowCoordinates(self, window, sim_x, sim_y):
        win_x = (window.getWidth()/self._width) * (sim_x - self._bounds["xMin"])
        win_y = (window.getHeight()/self._height) * (self._bounds["yMax"] - sim_y)
        return win_x, win_y

    def toSimulationCoordinates(self, window, pix_x, pix_y):
        sim_x = self._bounds["xMin"] + (self._width/window.getWidth())*pix_x #pixX-0
        sim_y = (window.getHeight()-pix_y)*self._height/window.getHeight()
        return sim_x, sim_y

    def toSimulationDimensions(self, window, pix_width, pix_height):
        return window.toSimulationDimensions(self, pix_width, pix_height)

    def toPixelDimensions(self, window, sim_width, sim_height):
        return window.toPixelDimensions(self, sim_width, sim_height)

class Rect:
    #xMin, xMax, yMin, yMax
    def __init__(self, x, y, width, height):
        self.xMin = x-width/2.
        self.xMax = x+width/2.
        self.yMin = y-height/2.
        self.yMax = y+height/2.

    def collidesWith(self, rect):
        return not (self.xMin>rect.xMax or self.xMax<rect.xMin
                    or self.yMin>rect.yMax or self.yMax<rect.yMin)