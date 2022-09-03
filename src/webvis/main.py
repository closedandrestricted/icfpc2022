#!/usr/bin/python3
import json
import os
from posixpath import join
import tornado.ioloop
import tornado.web


this_dir = os.path.dirname(os.path.abspath(__file__))


class MainHandler(tornado.web.RequestHandler):
    def prepare(self):
        header = "Content-Type"
        body = "text/html"
        self.set_header(header, body)

    def get(self):
        with open(os.path.join(this_dir, './static/index.html')) as f:
            data = f.read()
            self.write(data)


class ProblemHandler(tornado.web.RequestHandler):
    def prepare(self):
        header = "Content-Type"
        body = "image/png"
        self.set_header(header, body)

    def get(self):
        id = self.get_argument('id')
        path = os.path.join(this_dir, '../../problems/%s.png' % id)
        print("Serving " + path)
        with open(path, 'rb') as f:
            data = f.read()
            self.write(data)


class InitialHandler(tornado.web.RequestHandler):
    def prepare(self):
        header = "Content-Type"
        body = "application/json"
        self.set_header(header, body)

    def get(self):
        id = self.get_argument('id')
        path = os.path.join(this_dir, '../../problems/%s.initial.json' % id)
        if not os.path.exists(path):
            path = os.path.join(
                this_dir, '../../problems/default.initial.json')
        print("Serving " + path)
        with open(path, 'rb') as f:
            data = f.read()
            self.write(data)


class SolutionHandler(tornado.web.RequestHandler):
    def prepare(self):
        header = "Content-Type"
        body = "application/json"
        self.set_header(header, body)

    def get(self):
        id = self.get_argument('id')
        kind = self.get_argument('kind', 'best')
        path = os.path.join(this_dir, '../../solutions/%s/%s.txt' % (kind, id))
        if os.path.exists(path):
            print("Serving " + path)
            with open(path) as f:
                data = f.read()
                self.write(data)
        else:
            print("WARNING: no solution " + path)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/problem", ProblemHandler),
        (r"/solution", SolutionHandler),
        (r"/initial", InitialHandler),
        (r"/static/(.*)", tornado.web.StaticFileHandler,
         {"path": os.path.join(this_dir, "static")}),
    ], debug=True)


if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print('Go to http://localhost:8888')
    tornado.ioloop.IOLoop.current().start()
