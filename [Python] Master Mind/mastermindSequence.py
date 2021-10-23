from random import choice

def generateRandomSequence(pool, size):
    pool = list(pool)
    seq = []
    for i in range(size):
        seq.append(choice(pool))
        pool.remove(seq[i])
    return seq

def checkGuess(trueSeq, guess):
    correct, halfCorrect = 0, 0
    for i in range(len(trueSeq)):
        if trueSeq[i]==guess[i]:
            correct+=1
        elif trueSeq[i] in guess:
            halfCorrect+=1
    return guess+[correct, halfCorrect]