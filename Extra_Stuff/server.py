# Server stuff
from http.server import BaseHTTPRequestHandler, HTTPServer
from pynput.keyboard import Key, Controller
from http.client import HTTPConnection
from urllib.parse import urlparse
import json

# Threading stuff
import threading
import time


class ServerHandler(BaseHTTPRequestHandler):
    # Empty dictionary that contains status of all WiFi components that have spoken to it
    # Basically, its going to store information about which puzzles have been solved (not which havent)
    statusDict = {}
    statusLock = None # This will get set by external code, so dont worry about the fact its None here XD
    keyboardForVideoControl = Controller()

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
            return {'hint', 'This is a really good hint. Uncomment out the line when you ready to actually go for hints.'}
            #return ConnectToESP("192.168.1.211", 1234, f'play={hintGiven}', 5)


        # Else, process query commands
        query_components = {}
        query = urlparse(self.path).query

        # This is a little verbose, but essentially it just builds a dictionary out of the query string passed in
        if len(query) > 0:
            for qc in query.split("&"):
                queryParam = qc.split("=")
                query_components[queryParam[0]] = queryParam[1]

            # Save the statusDict first! So that once we activate the key presses
            #  the other bits of code can read out the correct values from the statusDict
            self.updateStatusDict(query_components)
            
            # Check for a clock time
            clockTime = query_components.get("clocktime", None)
            if clockTime == None:
                ...
            else:
                timeArray = clockTime.split(':')
                try:
                    # These are here for later, its easier just to press the same key as the hour for now
                    hour = int(timeArray[0])
                    min = int(timeArray[1])
                    self.__pressAndRelease(timeArray[0])
                except:
                    print("There was a problem parsing the hour or minute of the clocktime. Failing gracefully.")

            # Check for a clock mode
            clockmode = query_components.get("clockmode", None)
            if clockmode == None:
                ...
            elif clockmode == "fastForward":
                self.__pressAndRelease('w')
            elif clockmode == "reverse":
                self.__pressAndRelease('w')
            elif clockmode == "tick":
                self.__pressAndRelease('g')

            # Check if the monster is showing
            monster = query_components.get("monster", None)
            if monster == None:
                ...
            elif monster == "true":
                self.__pressAndRelease('m')
            elif monster == "false":
                self.__pressAndRelease('g')

            # Check if midnight should be showing
            midnight = query_components.get("midnight", None)
            if midnight == None:
                ...
            elif midnight == 'true':
                self.__pressAndRelease('f')

            # HEY NAMI! Come here if you need to add more query string commands
            #  that you would like the server to process (or if you need to change the keys that get pressed)


        # Tell the client their command was received
        return {'command':'received'}

    # This is just a helper function so I dont have to type press and release over and over XD
    def __pressAndRelease(self, key):
        self.keyboardForVideoControl.press(key)
        self.keyboardForVideoControl.release(key)

    def updateStatusDict(self, newStatusDict):
        self.statusLock.acquire()
        self.statusDict.update(newStatusDict)
        self.statusLock.release()

    def processHint():
        # TODO: Add some complicated logic so we can determine which hint should get sent back to the user
        return 0

class EscapeY2KServer:
    __statusDict = {}
    statusLock = threading.Lock()

    def __init__(self):
        ServerHandler.statusLock = self.statusLock
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

if __name__ == '__main__':
    # Start our HTTP Server
    server = EscapeY2KServer()
    server.startHTTPServer()

    # Handle other stuff that has to get done
    # Even if there is nothing else dont remove this because
    # the main thread has to keep running for the child thread to keep running
    while True:
        time.sleep(5)
        # print('escapey2k status')
        # server.PrintStatus()
        # print(ConnectToESP('10.0.0.94', 1234, 5))
