import os
import math
import time
import json

import busio
import board

import numpy as np
from scipy.interpolate import griddata

import colour

import adafruit_amg88xx

import websockets
import asyncio

i2c_bus = busio.I2C(board.SCL, board.SDA)

#low range of the sensor (this will be blue on the screen)
MINTEMP = 26.

#high range of the sensor (this will be red on the screen)
MAXTEMP = 32.

#how many color values we can have
COLORDEPTH = 1024

#initialize the sensor
sensor = adafruit_amg88xx.AMG88XX(i2c_bus)

# pylint: disable=invalid-slice-index
points = [(math.floor(ix / 8), (ix % 8)) for ix in range(0, 64)]
grid_x, grid_y = np.mgrid[0:7:32j, 0:7:32j]
# pylint: enable=invalid-slice-index

#the list of colors we can choose from
blue = colour.Color("indigo")
colors = list(blue.range_to(colour.Color("red"), COLORDEPTH))

#create the array of colors
colors = [(str(int(c.red * 255)), str(int(c.green * 255)), str(int(c.blue * 255))) for c in colors]

#some utility functions
def constrain(val, min_val, max_val):
    return min(max_val, max(min_val, val))
def map_value(x, in_min, in_max, out_min, out_max):           
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

async def send_json(websocket,path):
         while True:
            #read & send pixels
            pixels = []
            tempArray = []
            OutputColorArray = []
            for row in sensor.pixels:
                pixels += row
                tempArray += row
            pixels = [map_value(p, MINTEMP, MAXTEMP, 0, COLORDEPTH - 1) for p in pixels]

            #Find out the Max & Min value
            temp_min_value=np.min(tempArray)
            temp_max_value=np.max(tempArray)

            #perform interpolation
            bicubic = griddata(points, pixels, (grid_x, grid_y), method='cubic')

            #draw everything
            for ix, row in enumerate(bicubic):
                for jx, pixel in enumerate(row):
                    OutputColorArray.append('rgb('+','.join(colors[constrain(int(pixel), 0, COLORDEPTH- 1)])+')')

            OutputColorArray.append(temp_min_value)
            OutputColorArray.append(temp_max_value)
            await websocket.send(json.dumps(OutputColorArray))

start_server=websockets.serve(send_json,'192.168.2.138',8082)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
