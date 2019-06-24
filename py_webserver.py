import tornado.web

from tornado.web import RequestHandler
from tornado.websocket import WebSocketHandler
from tornado.ioloop import IOLoop

class IndexHandler(RequestHandler):  #返回控制页面
    def get(self):
        self.render("ws.html")

if __name__=="__main__":
    
    app=tornado.web.Application([
        (r'/', IndexHandler)
    ])
    app.listen(8081)
    IOLoop.current().start()