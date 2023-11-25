import random
numOfPuzzlesToUse = 3
selectedPuzzles = [4, 0, 2] # THIS IS JUST A PLACEHOLDER
solvedPuzzle = "dial"
puzzles = [
    {"name":"dial", "ip":"FILL IN", "port":1234},
    {"name":"bust", "ip":"FILL IN", "port":1234},
    {"name":"plugboard", "ip":"FILL IN", "port":1234},
    {"name":"potentiometer", "ip":"FILL IN", "port":1234},
    {"name":"cant remember lol", "ip":"FILL IN", "port":1234}
]

lockBoxes = [
    {"puzzleName":"dial", "ip":"FILL IN", "port":1234},
    {"puzzleName":"bust", "ip":"FILL IN", "port":1234},
    {"puzzleName":"plugboard", "ip":"FILL IN", "port":1234},
    {"puzzleName":"potentiometer", "ip":"FILL IN", "port":1234},
    {"puzzleName":"cant remember lol", "ip":"FILL IN", "port":1234}
]

def resetAndShuffle(puzzles, numOfPuzzlesToUse):
    return random.sample(range(0, len(puzzles)), numOfPuzzlesToUse)


def findIndexInList(lst, key, value):
    for i, dic in enumerate(lst):
        if dic[key] == value:
            return i
    return -1

# FOR TESTING
solvedPuzzle = puzzles[random.randint(0, len(puzzles)-1)]["name"]
selectedPuzzles = resetAndShuffle(puzzles, numOfPuzzlesToUse)
print(f'solvedPuzzle: {solvedPuzzle}')
print(f'After shuffle: {selectedPuzzles}')


# indexOfPuzzle is the index of the puzzle object in the puzzles list
indexOfPuzzle = findIndexInList(puzzles, "name", solvedPuzzle)
print(indexOfPuzzle)
# This checks whether the puzzle exists in the puzzles list
if(indexOfPuzzle != -1):
    try:
        indexInSelectedList = selectedPuzzles.index(indexOfPuzzle)
    except ValueError:
        indexInSelectedList = -1

    # Is the puzzle even in the list of puzzles that the player has to solve?
    # If its not it effectively doesnt matter, so dont do anything
    if(indexInSelectedList != -1):
        lockboxIndex = findIndexInList(lockBoxes, "puzzleName", puzzles[indexOfPuzzle]["name"])
        print(f'Lockbox dict: {lockBoxes[lockboxIndex]}')
    else:
        # TODO: Send a message to the tape player so itll congratulate them for solving the puzzle, but also let them know that we didnt need that one solved
        print("We dont care if that puzzle was solved or not :P")