# I dont think I am going to use this, but I added it here to keep track of it

from http.server import BaseHTTPRequestHandler, HTTPServer
import os.path
        

class Server(BaseHTTPRequestHandler):

    def do_GET(self):
        return self.router()

    def router(self):
        print("hi!")
        path = self.path.removeprefix("/")
        if os.path.exists(path):
            response_code, body = self.controller(path)
        else:
            body = "Don't know what page you want"
            response_code = 404

        self.send_response(response_code)
        if path.endswith(".css"):
            self.send_header("Content-Type", "text/css")
        else:
            self.send_header("Content-Type", "text/html")
        self.end_headers()

        self.wfile.write(body.encode("utf8"))

    def controller(self, path):
        visitor_count = Model().read_visitor_count()
        response_code = 200
        body = self.view(path, visitor_count)
        Model().write_visitor_count(visitor_count + 1)
        return response_code, body

    def view(self, template_file, visitor_count):
        return open(template_file).read().replace("%COUNT%", str(visitor_count))


class Model:
    def read_visitor_count(self):
        with open("counter.txt") as f:
            return int(f.read())
    
    def write_visitor_count(self, visitor_count : int):
        with open("counter.txt", "w") as f:
            f.write(str(visitor_count))

if __name__ == '__main__':
    httpd = HTTPServer(('', 8001), Server)
    httpd.serve_forever()
