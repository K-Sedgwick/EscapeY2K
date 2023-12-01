# Server stuff
from http.server import BaseHTTPRequestHandler, HTTPServer
from pynput.keyboard import Key, Controller
from http.client import HTTPConnection
from urllib.parse import urlparse
import json
import random

# Threading stuff
import threading
import time

# Rando connection stuff
from multiprocessing import Process, Pipe


class ServerHandler(BaseHTTPRequestHandler):
    # These are for helping us keep track of which puzzles are going to be used in that playthrough of the room
    numOfPuzzlesToUse = 3
    selectedPuzzles = [1, 2, 3] # THIS IS JUST A PLACEHOLDER
    puzzles = [
        {"name":"dial", "ip":"FILL IN", "port":1234, "hintNum":0},
        {"name":"bust", "ip":"FILL IN", "port":1234, "hintNum":1},
        {"name":"plugboard", "ip":"FILL IN", "port":1234, "hintNum":2},
        {"name":"potentiometer", "ip":"FILL IN", "port":1234, "hintNum":3},
        {"name":"cant remember lol", "ip":"FILL IN", "port":1234, "hintNum":4}
    ]
    lockBoxes = [
        {"puzzleName":"TBD", "ip":"192.168.1.127", "port":1234},
        {"puzzleName":"TBD", "ip":"192.168.1.242", "port":1234},
        {"puzzleName":"TBD", "ip":"192.168.1.143", "port":1234},
        {"puzzleName":"TBD", "ip":"192.168.1.54", "port":1234},
        {"puzzleName":"TBD", "ip":"192.168.1.150", "port":1234}
    ]
    # Empty dictionary that contains status of all WiFi components that have spoken to it
    # Basically, its going to store information about which puzzles have been solved (not which havent)
    statusDict = {"puzzlesToSolve":selectedPuzzles}
    statusLock = None # This will get set by external code, so dont worry about the fact its None here XD
    child_conn = None # This will also get set by external code, so we can use it without worry
    keyboardForVideoControl = Controller()
    clock = {"ip":"192.168.1.50", "port":1234}

    def do_GET(self):
        return self.router()

    def router(self):

        dictToReturn = self.processQueryComponents()
        response_code = 200

        self.send_response(response_code)
        self.send_header("Content-Type", "application/json")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET')
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')

        self.end_headers()

        self.wfile.write(json.dumps(dictToReturn).encode('utf-8'))

    '''
        Okay, this method needs a lot of explanation
            1. This essentially breaks apart the path and query string to get information and do stuff with it
            2. The part that deals with the query string performs multiple checks for various variables. We check for...
                a. Clockmode, to see what the clock is doing and change what the TVs are showing a cambio
                b. Monster, to see if the monster should be shown on the screen or not
                c. Midnight, to see if we should show the midnight scene
    '''
    def processQueryComponents(self):
        # First, check if status is in the path and return the status object if it is
        if self.path == '/status':
            self.statusLock.acquire()
            copyOfStatus = self.statusDict.copy()
            self.statusLock.release()
            return copyOfStatus
        elif self.path == '/hint':
            hintGiven = self.processHint()
            print(f'The hint given was hint {hintGiven}')
            return {'hint', hintGiven}
            #return ConnectToESP("192.168.1.211", 1234, f'play={hintGiven}', 5)
        elif self.path == '/shuffle':
            self.resetAndShuffle()
            return {'shuffleAndReset':'success'}


        # Else, process query commands
        query_components = {}
        query = urlparse(self.path).query

        # This is a little verbose, but essentially it just builds a dictionary out of the query string passed in
        if len(query) > 0:
            for qc in query.split("&"):
                queryParam = qc.split("=")
                # If the query key is "solved" add the solved puzzle names to a list with the key "solved"
                if queryParam[0] == 'solved':
                    # If that key already exists just append the value to the list of others
                    solvedInQueryComp = query_components.get("solved", None)
                    if(solvedInQueryComp != None):
                        query_components[queryParam[0]].append(queryParam[1])
                    else:
                        query_components[queryParam[0]] = [queryParam[1]]
                else:
                    query_components[queryParam[0]] = queryParam[1]


            # Save the statusDict first! So that once we activate the key presses
            #  the other bits of code can read out the correct values from the statusDict
            self.updateStatusDict(query_components)
            
            # Checking for the "clocktime" command has been deprecated until TBD
            # # Check for a clock time
            # clockTime = query_components.get("clocktime", None)
            # if clockTime == None:
            #     ...
            # else:
            #     timeArray = clockTime.split(':')
            #     try:
            #         # These are here for later, its easier just to press the same key as the hour for now
            #         hour = int(timeArray[0])
            #         min = int(timeArray[1])
            #         self.__pressAndRelease(timeArray[0])
            #     except:
            #         print("There was a problem parsing the hour or minute of the clocktime. Failing gracefully.")

            # Check for a clock mode
            clockmode = query_components.get("clockmode", None)
            if clockmode == None:
                ...
            elif clockmode == "fastForward":
                self.__pressAndRelease('w')
                ConnectToESP(self.clock["ip"], self.clock["port"], "fastForward=on", 5000)
            elif clockmode == "reverse":
                self.__pressAndRelease('w')
                ConnectToESP(self.clock["ip"], self.clock["port"], "reverse=on", 5000)
            elif clockmode == "tick":
                self.__pressAndRelease('g')
                initializeRoom = False
                if(initializeRoom):
                    ... # TODO: ADD STUFF HERE
                else:
                    responseFromESP = ConnectToESP(self.clock["ip"], self.clock["port"], "normal=on", 5000)
                    try:
                        responseDict = json.loads(responseFromESP)
                        # OPTION 1
                        timeRange = int(responseDict["rotaryCounter"])//10
                        self.__pressAndRelease(f"{timeRange}")

                        # OPTION 2
                        # First, get the values that the time it going to be set to
                        rotaryCounter = int(responseDict["rotaryCounter"])
                        videoPositionPercent = (rotaryCounter*2)/180
                        # Update that value so the server can read it
                        self.updateStatusDict({"videoPositionPercent":videoPositionPercent})
                        # Then tell the TV to update the position using that value

                    except ValueError as e:
                        # If parsing the response doesnt work just dont change the time "shrug"
                        ...

            # Check if the monster is showing
            monster = query_components.get("monster", None)
            if monster == None:
                ...
            elif monster == "seek":
                self.__pressAndRelease('s')
            elif monster == "true":
                self.__pressAndRelease('m')
            elif monster == "false":
                self.__pressAndRelease('g')

            # Check if the monster is showing
            keypad = query_components.get("keypad", None)
            if keypad == None:
                ...
            elif keypad == "increment":
                self.__pressAndRelease('+')
            # Decrement works a bit different, it makes the timer go down faster when '-' is pressed down and held.
            elif keypad == "decrement":
                self.__pressAndRelease('m')

            # Check if midnight should be showing
            midnight = query_components.get("midnight", None)
            if midnight == None:
                ...
            elif midnight == 'true':
                self.__pressAndRelease('f')

            # When a puzzle is solved, receive this command
            solvedPuzzle = query_components.get("solved", None)
            if solvedPuzzle == None:
                ...
            else:
                self.processNextStep(solvedPuzzle)

            # When telling the room to reset, execute the code here
            reset = query_components.get("reset", None)
            if midnight == None:
                ...
            elif midnight == 'reset':
                self.__pressAndRelease('}')

            # HEY NAMI! (Hi Jake :D) Come here if you need to add more query string commands
            #  that you would like the server to process (or if you need to change the keys that get pressed)


        # Tell the client their command was received
        return {'command':'received'}

    # This method is in charge of processing the puzzle that was just solved, which box it should open
    #   (essentially what solving that puzzle does for the player), and which hint should be given to the players
    def processNextStep(self, solvedPuzzle):
        # Although solvedPuzzle is an array it should NEVER pass in more than one puzzle at a time!
        # indexOfPuzzle is the index of the puzzle object in the puzzles list
        indexOfPuzzle = self.findIndexInList(self.puzzles, "name", solvedPuzzle[0])
        print(indexOfPuzzle)
        # This checks whether the puzzle exists in the puzzles list
        if(indexOfPuzzle != -1):
            try:
                indexInSelectedList = self.selectedPuzzles.index(indexOfPuzzle)
            except ValueError:
                indexInSelectedList = -1

            # Is the puzzle even in the list of puzzles that the player has to solve?
            # If its not it effectively doesnt matter, so dont do anything
            if(indexInSelectedList != -1):
                lockboxIndex = self.findIndexInList(self.lockBoxes, "puzzleName", self.puzzles[indexOfPuzzle]["name"])
                print(f'Lockbox dict: {self.lockBoxes[lockboxIndex]}')
            else:
                # TODO: Send a message to the tape player so itll congratulate them for solving the puzzle, but also let them know that we didnt need that one solved
                print("We dont care if that puzzle was solved or not :P")
    
    def processHint(self):
        # Compare the status solved and the status puzzlesToSolve to see which ones are missing, then somehow decide which one to give a hint for?
        # Compare indexes of the names of the solved puzzles to find which one is next and get the hint from there
        return 0
    
    # Once we call this method, this resets the room and changes the puzzles that need to be solved in order for the room to be completable
    def resetAndShuffle(self):
        # Reset the list of currently selected puzzles
        self.selectedPuzzles.clear()
        self.selectedPuzzles = random.sample(range(0, len(self.puzzles)), self.numOfPuzzlesToUse)

        print(self.selectedPuzzles)
        # TODO: Then tell all of the espModules to reset

        # And reset the statusDict, so its as if we just started from zero
        self.clearStatusDict()
        listOfNames = []
        for val in self.selectedPuzzles:
            listOfNames.append(self.puzzles[val]["name"])
        self.updateStatusDict({"puzzlesToSolve":listOfNames})

        # Also, make sure to tell the TVs to go back to black (FORGET THE HERSE CUZ I NEVER DIE)
        self.__pressAndRelease('d') # d for dark

    # HELPER FUNCTIONS
    # This is just a helper function so I dont have to type press and release over and over XD
    def __pressAndRelease(self, key):
        self.keyboardForVideoControl.press(key)
        self.keyboardForVideoControl.release(key)
    
    def clearStatusDict(self):
        self.statusLock.acquire()
        self.statusDict.clear()
        self.statusLock.release()

    def updateStatusDict(self, newStatusDict):
        self.statusLock.acquire()
        # Since the "solved" puzzles are stored in a list we have to perform some extra checks to make sure that updates correctly
        # If solved doesnt exist in the statusDict just update normally
        if(self.statusDict.get("solved", None) == None):
            self.statusDict.update(newStatusDict)
        else:
            # If it does, save it so we can append it after the update action
            oldSolvedList = self.statusDict.get("solved", None)
            self.statusDict.update(newStatusDict)
            # Add each of the puzzles previously in the list back to the list (and make sure theyre not duplicates)
            for solvedPuzzle in oldSolvedList:
                self.statusDict["solved"].append(solvedPuzzle) if solvedPuzzle not in self.statusDict["solved"] else self.statusDict["solved"]
        self.statusLock.release()

    def findIndexInList(self, lst, key, value):
        for i, dict in enumerate(lst):
            if dict[key] == value:
                return i
        return -1
            


class EscapeY2KServer:
    __statusDict = {}
    child_conn = None
    statusLock = threading.Lock()

    def __init__(self, child_conn):
        ServerHandler.statusLock = self.statusLock
        self.child_conn = child_conn
        ServerHandler.child_conn = child_conn
        self.__statusDict = ServerHandler.statusDict

    def startHTTPServer(self):
        httpd = HTTPServer(('', 8001), ServerHandler)
        serverThread = threading.Thread(target=httpd.serve_forever)
        serverThread.daemon = True
        serverThread.start()

    def PrintStatus(self):
        self.statusLock.acquire()
        print(self.__statusDict)
        self.statusLock.release()

    def GetStatus(self):
        self.statusLock.acquire()
        copyOfStatus = self.__statusDict.copy()
        self.statusLock.release()
        return copyOfStatus
    
    def SetStatus(self, newStatusDict):
        self.statusLock.acquire()
        self.__statusDict = newStatusDict
        self.statusLock.release()


# TODO: Maybe change this so it throws the error up to the method that called it instead of just returning a string
def ConnectToESP(ip, port, command, timeout):
    return {"test":True}
    try:
        headers = {'Content-type': 'application/json'}
        esp = HTTPConnection(ip, port, timeout=timeout)
        esp.request("GET", f'/?{command}', headers=headers)
        response = esp.getresponse()
        jsonFromESP = response.read().decode()
        return jsonFromESP
    except TimeoutError:
        timeoutString = "Connection Timeout"
        print(timeoutString)
        return timeoutString
    except AttributeError:
        attributeErrorString = "AttributeError? Not sure what went wrong tbh"
        print(attributeErrorString)
        return attributeErrorString

def InitiateServer(child_conn = None):
    # Start our HTTP Server
    server = EscapeY2KServer(child_conn)
    server.startHTTPServer()

    # Handle other stuff that has to get done
    # Even if there is nothing else dont remove this because
    # the main thread has to keep running for the child thread to keep running
    while True:
        time.sleep(5)
        # print('escapey2k status')
        # server.PrintStatus()
        # print(ConnectToESP('10.0.0.94', 1234, 5))

if __name__ == '__main__':
    InitiateServer()