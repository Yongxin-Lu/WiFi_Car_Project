# -*- coding: utf-8 -*
import serial
import time
# 打开串口
ser = serial.Serial("/dev/ttyAMA0", 115200)
def main():
    while True:
        count = ser.inWaiting()
        recv = ser.read(count)
        print(recv)
        # 清空接收缓冲区
        ser.flushInput()
        # 必要的软件延时
        time.sleep(0.1)

if __name__ == '__main__':
        main()
