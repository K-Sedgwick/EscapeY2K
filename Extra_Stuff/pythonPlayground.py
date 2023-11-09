from http.server import BaseHTTPRequestHandler,HTTPServer

class test:
    def show(self):
        return "aaaa"

class http_server:
    def __init__(self, t1):
        myHandler.t1 = t1
        server = HTTPServer(('', 8080), myHandler)
        server.serve_forever()

class myHandler(BaseHTTPRequestHandler):
    t1 = None
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        self.wfile.write(self.t1.show())
        return

class main:
    def __init__(self):
        self.t1 = test()

        self.server = http_server(self.t1)

if __name__ == '__main__':
    m = main()
    print(m.t1)