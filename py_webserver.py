import tornado.web
import _thread
import time

from tornado.web import RequestHandler
from tornado.ioloop import IOLoop

class IndexHandler(RequestHandler):
    def get(self):
        self.render("ws.html")

def thread_sayhi(threadName):
    while True:
        print("hi,simon")
        time.sleep(1)
        print("nice to meet u")
        time.sleep(1)

if __name__=="__main__":
    _thread.start_new_thread( thread_sayhi, ("Thread-1",) )   
    app=tornado.web.Application([
        (r'/', IndexHandler),
    ])
    app.listen(8081)
    IOLoop.current().start()