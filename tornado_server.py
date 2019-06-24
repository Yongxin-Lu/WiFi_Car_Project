import serial
import tornado.web

from tornado.web import RequestHandler
from tornado.websocket import WebSocketHandler
from tornado.ioloop import IOLoop

class IndexHandler(RequestHandler):
    def get(self):
        self.render("ws.html")

class wsHandler(WebSocketHandler):
    def open(self):
        self.write_message("OK")
    def on_message(self,message):
        if message.find("#")==0 or message.find("$")==0:
            message+="\r\n"
            ser.write(message.encode("utf-8"))
            #print(message)         
        elif message.find("@")==0:
            count=ser.inWaiting()
            recv=ser.read(count)
            self.write_message("@"+bytes.decode(recv)[0:3])
            ser.flushInput


if __name__=="__main__":
    ser = serial.Serial("/dev/ttyAMA0", 115200)  # 初始化串口
    ser.flushInput()

    app=tornado.web.Application([
        (r'/', IndexHandler),
        (r'/ws', wsHandler)
    ])
    app.listen(8081)
    IOLoop.current().start()