# Server stuff
from http.server import BaseHTTPRequestHandler, HTTPServer
from http.client import HTTPConnection, HTTPSConnection
from urllib.parse import urlparse, parse_qs
import json

# Threading stuff
import threading
import time
        

class Server(BaseHTTPRequestHandler):

    def do_GET(self):
        return self.router()

    def router(self):
        # Get the query from the path
        query_components = parse_qs(urlparse(self.path).query)

        # Then parse it and see if the value we are looking for exists
        clockmode = query_components.get("clockmode", None)
        if clockmode is not None:
            print(clockmode)
        else:
            print("Sorry, clockmode wasnt on the query string")

        clocktime = query_components.get("clocktime", None)
        if clocktime is not None:
            print(clocktime)
        else:
            print("No clock time given")
    
        response_code = 200

        self.send_response(response_code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()

        self.wfile.write(json.dumps({'clocktime': clocktime, 'clockmode': clockmode}).encode('utf-8'))


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
    while True:
        time.sleep(5)
        print(GetDataFromESP('10.0.0.94', 1234, 5))
