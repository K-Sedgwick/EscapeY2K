# Server stuff
from http.server import BaseHTTPRequestHandler, HTTPServer
from http.client import HTTPConnection, HTTPSConnection
from urllib.parse import urlparse, parse_qs
import json

# Threading stuff
import threading
import time

'''

Okay, so here is my idea. I want to have the server process commands from every one of the ESP modules.
This will allow it to create a "status of the room" object (probs a dictionary) which will contain info
about each of the modules and what state it is in. Then, the console can just call "state=get" to the TVs
and that will update every single object on the control panel at once.

This will also allow us to process hints a bit easier because the TVs will know the state of every object
in the room all the time. Once a hint is asked for the TVs will send a number response based on everything
else that is happening and the number will be the track number on the SD card that should be played.


'''

statusDictInMain = {}
statusLock = threading.Lock()
        

class Server(BaseHTTPRequestHandler):
    # Empty dictionary that contains status of all WiFi components that have spoken to it
    # Basically, its going to store information about which puzzles have been solved (not which havent)
    statusDict = {}

    def do_GET(self):
        return self.router()

    def router(self):

        dictToReturn = self.processQueryComponents();
            
        response_code = 200

        self.send_response(response_code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()

        # Maybe add more to this JSON? But I dont think the response needs to be very complicated
        self.wfile.write(json.dumps(dictToReturn).encode('utf-8'))

    
    def processQueryComponents(self):
        # First, check if status is in the path and return the status object if it is
        if self.path == '/status':
            return self.statusDict

        # Else, process query commands
        query_components = {}
        query = urlparse(self.path).query

        # This is a little verbose, but essentially it just builds a dictionary out of the query string passed in
        for qc in query.split("&"):
            queryParam = qc.split("=")
            query_components[queryParam[0]] = queryParam[1]

        self.statusDict.update(query_components)

        # Update the statusDictInMain so everyone can see whats going on, but try to it safely lol
        global statusLock
        statusLock.acquire()
        global statusDictInMain
        statusDictInMain = self.statusDict
        statusLock.release()

        # Tell the client their command was received
        return {'command':'received'}

            

    def processHint():
        # TODO: Add some complicated logic so we can determine which hint should get sent back to the user
        return 0


def startHTTPServer():
    httpd = HTTPServer(('', 8001), Server)
    serverThread = threading.Thread(target=httpd.serve_forever)
    serverThread.daemon = True
    serverThread.start()

# TODO: Maybe change this so it throws the error up to the method that called it instead of just returning a string
def GetDataFromESP(ip, port, timeout):
    try:
        headers = {'Content-type': 'application/json'}
        esp = HTTPConnection(ip, port, timeout=timeout)
        esp.request("GET", "/?clockmode=get", headers=headers)
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
    startHTTPServer()

    # Handle other stuff that has to get done
    # Even if there is nothing else dont remove this because
    # the main thread has to keep running for the child thread to keep running
    while True:
        time.sleep(5)
        statusLock.acquire()
        print('current status', statusDictInMain)
        statusLock.release()
        # print(GetDataFromESP('10.0.0.94', 1234, 5))
