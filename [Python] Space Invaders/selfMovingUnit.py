import unit

#Event Handling
_FIELD, _AXIS, _OPERATION, _VALUE = 0, 1, 2, 3
_instructionHandler = {
    '+' : lambda field, value: field+value,
    '*' : lambda field, value: field*value,
}
STD_ENEMY_XBOUND_EVT = [('coord', 'y', '+', -1), ('direction', 'x', '*', -1)]
STD_ENEMY_YBOUND_EVT = [('direction', 'y', '*', -1)]
STD_BULLET_YBOUND_EVT = ['dispose']

class SelfMovingUnit(unit.Unit):
    evtBounds = {}
    #bounds = {
    #   on(X/Y)Bound: [ ((coord/direction), (x/y), number), ... ]
    # }

    def __init__(self, x, y, simulation, *, side, width=1, height=1, speed=1, direction=None, type='enemy', name, **bounds):
        super().__init__(x, y, simulation, side=side, width=width, height=height, speed=speed, direction=direction, type=type, name=name)
        #if no (x/y)(min/max)bound is given, simulation is used
        for k in set(simulation._bounds.keys()).difference(set(bounds.keys())):
            bounds[k] = simulation._bounds[k]
        self.evtBounds.update(bounds)

    def update(self, timeElapsed=1):
        # Movement
        super().update(timeElapsed)

        if self.direction['x'] != 0:
            if self.coord['x'] - self.width <= self.evtBounds["xMin"]:
                self.coord['x'] = self.evtBounds["xMin"] + self.width
                self.boundEvt('onXBound')
                #self.direction['x'] *= -1
            elif self.coord['x'] + self.width >= self.evtBounds["xMax"]:
                self.coord['x'] = self.evtBounds["xMax"] - self.width
                self.boundEvt('onXBound')
                #self.direction['x'] *= -1

        if self.direction['y'] != 0:
            if self.coord['y'] - self.height <= self.evtBounds["yMin"]:
                self.coord['y'] = self.evtBounds["yMin"] + self.height
                self.boundEvt('onYBound')
                #self.direction['y'] *= -1
            elif self.coord['y'] + self.height >= self.evtBounds["yMax"]:
                self.coord['y'] = self.evtBounds["yMax"] - self.height
                self.boundEvt('onYBound')
                #self.direction['y'] *= -1

    def boundEvt(self, evt):
        if evt in self.evtBounds:
            for op in self.evtBounds[evt]:
                if type(op)==tuple:
                    if op[_FIELD]=='coord':
                        self.coord[ op[_AXIS] ] = \
                            _instructionHandler[ op[_OPERATION] ] (self.coord[ op[_AXIS]], op[_VALUE] )
                    elif op[_FIELD]=='direction':
                        self.direction[ op[_AXIS] ] = \
                            _instructionHandler[ op[_OPERATION] ] (self.direction[ op[_AXIS]], op[_VALUE] )
                    else:
                        print("Unknown field ",op[_FIELD], "for evt", evt, "\n"
                                "\tInstructions were: ", op)
                else:
                    if op=='dispose':
                        self.dispose()
                    else:
                        print("Unknown operation ", op)

    def fireBullet(self, direction={'x':0, 'y':-1}):
        super().fireBullet(direction)