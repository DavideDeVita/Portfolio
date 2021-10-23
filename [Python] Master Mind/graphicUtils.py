import pygame
from pygame.rect import Rect

_ALLOW_SMOOTH_RESIZE=False

font = {}

def initFont():
    global font
    font['title'] = pygame.font.SysFont("inkfree", 48, bold=True)
    font['button'] = pygame.font.SysFont("inkfree", 32)
    font['end'] = pygame.font.SysFont("inkfree", 96, bold=True)

#loads an image
def loadImage(name):
    return pygame.image.load("res/"+name + ".png")

#loads an image
def resizeImage(img, newSize):
    if(_ALLOW_SMOOTH_RESIZE):
        return pygame.transform.smoothscale(img, newSize)
    else:
        return pygame.transform.scale(img, newSize)

#updates the screen display in the game loop
def updateView():
    pygame.display.update()

def renderString(fontName, string, color):
    return font[fontName].render(string, True, color)

#drawing
BLACK_OFFSET=0.03
STRIPE_EVAL_RATIO=0.25
STRIPE_GUESS_RATIO= 1 - STRIPE_EVAL_RATIO
TITLE_OFFSET_RATIO=0.125
RADIUS_RATIO = 0.3 #of Offset
EXTRA_RADIUS = 1.25
CURRENT_GUESS_HEIGHT_RATIO = 0.15
BUTTON_WIDTH_RATIO, BUTTON_HEIGHT_RATIO = 0.5, 0.15
color={
    "white" : (255,255,255), "black" : (0,0,0),
    "dark brown" : (71, 35, 10), "brown" : (169, 39, 19),
    #selectables
    "yellow" : (255, 255, 0), "red" : (255, 0, 0), 'pink':(255,128,128),
    "orange" : (255, 128, 0), "green" : (0, 255, 0), "azul" : (0, 255, 255),
    "purple" : (128, 0, 128), "blue" : (0, 0, 255), 'grey': (128,128,128)
}
selectableColors = list(color.keys())[4:]

lastClickables = []

def drawMastermindBoard(window, *, previousGuesses, currentGuess, selected, renderLeft=True, renderGuess=True, renderAll=True):
    if renderAll:
        window.clear()
    w, h = window.getWidth(), window.getHeight()
    if renderAll:
        drawLine( window, start=(w/2, 0), end=(w/2, h), color=color['white'] )

    if renderLeft or renderAll:
        drawLeftSide(window, previousGuesses, w/2, h)
    if renderGuess or renderAll:
        clickables = drawRightSide(window, currentGuess, selected, w/2, h, renderAll)
        if renderAll:
            global lastClickables
            lastClickables=clickables
    return lastClickables

def drawLeftSide(window, previousGuesses, halfWidth, h):
    w=halfWidth
    #Left
    board = Rect(w * BLACK_OFFSET, h * BLACK_OFFSET, (1 - 2 * BLACK_OFFSET) * w, (1 - 2 * BLACK_OFFSET) * h)
    drawRect( window, rect = board, color = color['dark brown'] )
    #Hor stripes
    stripesHeight = board.height/7
    for row in range(1, 7):
        drawLine(window,
             start=(board.left, board.top+row*stripesHeight), end=(board.right, board.top+row*stripesHeight),
             color=color['brown'], width=1
            )
    #Vertical stripe
    vLine_x = board.left + board.width * STRIPE_EVAL_RATIO
    drawLine(window,
             start=(vLine_x, board.top + stripesHeight),
             end=(vLine_x, board.bottom),
             color=color['brown'], width=1
             )
    #Title
    window.write("title", "Master Mind",
             pixX=board.left+board.width*TITLE_OFFSET_RATIO, pixY=board.top+stripesHeight*TITLE_OFFSET_RATIO,
             color=color['brown']
         )
    #Previous guesses
    row=6
    guessesOffset = board.width*STRIPE_GUESS_RATIO/5
    guess_radius = guessesOffset*RADIUS_RATIO
            #evaluation variables
    x_incr = board.width*STRIPE_EVAL_RATIO/2
    y_incr = stripesHeight/2
    eval_radius = min(x_incr, y_incr) * RADIUS_RATIO
    for guess in previousGuesses:
        #guesses
        stripeHH = board.top+(row+0.5)*stripesHeight
        for g in range(1,5):
            drawCircle(window, center=(vLine_x + g*guessesOffset, stripeHH), radius=guess_radius, color=color[ guess[g-1] ])
        #evaluation
        correct, halfCorrect = guess[4], guess[5]
        eval_cX = board.left + board.width*STRIPE_EVAL_RATIO/4
        eval_cY = board.top + row*stripesHeight + stripesHeight/4
        x_incr_count, y_incr_count = 0, 0
        while correct>0:
            drawCircle(window, center=(eval_cX+ x_incr*x_incr_count, eval_cY + y_incr*y_incr_count),
                   radius = eval_radius, color=color['white']
               )
            x_incr_count+=1
            if x_incr_count>1:
                x_incr_count=0
                y_incr_count+=1
            correct-=1
        while halfCorrect>0:
            drawCircle(window, center=(eval_cX+ x_incr*x_incr_count, eval_cY + y_incr*y_incr_count),
                   radius = eval_radius, color=color['black']
               )
            x_incr_count+=1
            if x_incr_count>1:
                x_incr_count=0
                y_incr_count+=1
            halfCorrect-=1

        row-=1

def drawRightSide(window, currentGuess, selected, halfWidth, h, renderClickables):
    clickables = []
    w=halfWidth
    #current guess
    stripe = rect=Rect(w + BLACK_OFFSET*w, BLACK_OFFSET*h, w - 2*BLACK_OFFSET*w, CURRENT_GUESS_HEIGHT_RATIO*h)
    drawRect(window, rect=rect, color=color['dark brown'])

    cY = stripe.top + stripe.height/2
    cX_offset = stripe.width/5
    radius = cX_offset*RADIUS_RATIO
    for i in range(1,5):
        if i==selected:
            drawCircle(window, center=(stripe.left + cX_offset * i, cY), color=color['white'], radius=radius*EXTRA_RADIUS)

        color_i = color['black'] if currentGuess[i-1] is None else color[currentGuess[i-1]]
        drawCircle(window, center=(stripe.left + cX_offset*i, cY), color=color_i, radius=radius )

        circle = Rect(stripe.left + cX_offset * i - radius, cY - radius, 2 * radius, 2 * radius)
        if renderClickables:     #Unnecessary if not resized
            clickables.append({"rect": circle, "action": "select", "select": i})

    if renderClickables:  # Unnecessary if not resized
        #guess button
        button = Rect(w + w*BUTTON_WIDTH_RATIO/2, h - BUTTON_HEIGHT_RATIO*h - BLACK_OFFSET*h, w*BUTTON_WIDTH_RATIO, BUTTON_HEIGHT_RATIO*h)
        drawRect(window, rect=button, color=color['white'])
        window.write("button", "Guess !", pixX=button.left + button.width*0.3, pixY=button.top + button.height*0.25, color=color['black'])
        clickables.append({"rect":button, "action":"guess"})

        #color possibilities
        left=w + BLACK_OFFSET*w; right = 2*w - BLACK_OFFSET*w
        top = stripe.bottom+ BLACK_OFFSET*h*2; bottom = button.top - BLACK_OFFSET*h*2
        boxW, boxH = (right-left)/3, (bottom-top)/3
        from random import choice
        for r in range(3):
            for c in range(3):
                box = Rect(left + c*boxW, top + r*boxH, boxW, boxH)
                drawRect(window, rect=box, color=color[ selectableColors[r*3 + c] ])
                clickables.append({"rect":box, "action":"change color", "color":selectableColors[r*3 + c]})

    return clickables

ENDING_STRIPE_WIDTH_RATIO =  0.8
ENDING_STRIPE_HEIGHT_RATIO =  0.4
def gameWon(window):
    drawRect(window,
        rect=Rect((1-ENDING_STRIPE_WIDTH_RATIO)*window.getWidth()/2, (1-ENDING_STRIPE_HEIGHT_RATIO)*window.getHeight()/2,
             ENDING_STRIPE_WIDTH_RATIO*window.getWidth(), ENDING_STRIPE_HEIGHT_RATIO*window.getHeight()),
         color = color['white']
     )
    window.write("end", "You Win", 0.3*window.getWidth(), 0.4*window.getHeight(), color['green'])

def gameLost(window, secretSequence):
    drawRect(window,
        rect=Rect(
            window.getWidth()/2 + (1-ENDING_STRIPE_WIDTH_RATIO)*window.getWidth()/4,
            (1-ENDING_STRIPE_HEIGHT_RATIO)*window.getHeight()/2,
             ENDING_STRIPE_WIDTH_RATIO*window.getWidth()/2, ENDING_STRIPE_HEIGHT_RATIO*window.getHeight()),
         color = color['white']
     )
    window.write("end", ":(", 0.55*window.getWidth(), 0.4*window.getHeight(), color['black'])
    window.write("end", "):", 0.85*window.getWidth(), 0.4*window.getHeight(), color['black'])
    left = window.getWidth()*0.6; right=window.getWidth()*0.9
    top = (1-ENDING_STRIPE_HEIGHT_RATIO)*window.getHeight()/2; bottom= (1+ENDING_STRIPE_HEIGHT_RATIO)*window.getHeight()/2
    x_offset = (right-left)/5
    cY = (top+bottom)/2
    i=1
    for c in secretSequence:
        drawCircle(window, center=(left + x_offset*i, cY), radius=x_offset*RADIUS_RATIO, color=color[c])
        i+=1

def drawRect(window, *, rect, color):
    pygame.draw.rect(surface=window._window, color=color, rect=rect)

def drawCircle(window, *, center, radius, color):
    pygame.draw.circle(surface=window._window, color=color, center=center, radius=radius)

def drawLine(window, *, start, end, color, width=1):
    pygame.draw.line(surface=window._window, color=color, start_pos=start, end_pos=end, width=width)