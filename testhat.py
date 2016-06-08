#!/usr/bin/env python3
#
# testhat.py
#
# Emulate a unicorn hat and monohorn
#

import sys
from liblo import Server, make_method
from tkinter import Tk, Canvas

class Matrix:
    def __init__(self, rows=8, cols=8):
        self.updates = []
        self.rows = rows
        self.cols = cols
        self.root = Tk()
        self.canvas = Canvas(self.root, width=400, height=400)
        self.canvas.pack()

        rw = int(self.canvas.config()['width'][-1]) / cols
        rh = int(self.canvas.config()['height'][-1]) / rows

        self.leds = {}

        for c in range(cols):
            for r in range(rows):
                x = rw * c
                y = rh * r
                self.leds[(c,r)] = self.canvas.create_rectangle(x, y, x+rw, y+rh, fill='black')

    def set_led(self, x, y, color='black'):
        self.canvas.itemconfig(self.leds[(x,y)], fill=color)


class MonoHorn(Server):
    def __init__(self, socket_path):
        super(MonoHorn, self).__init__(socket_path)
        self.matrix = Matrix()
        self.matrix.root.after(0, self.update)
        self.matrix.root.mainloop()

    @make_method('/set', 'iii')
    def set_handler(self, path, args):
        print(path, args)
        x, y, clr_int = args
        clr = '#' + hex(clr_int)[2:].rjust(6, '0')
        if x >= 0 and x < self.matrix.cols and y >= 0 and y < self.matrix.rows:
            self.matrix.updates.append((x, y, clr))

    @make_method('/render', None)
    def render_handler(self, path, args):
        while len(self.matrix.updates) > 0:
            x, y, clr = self.matrix.updates.pop()
            print(path, x, y, clr)
            self.matrix.set_led(x, y, color=clr)

    @make_method('/clear', None)
    def clear_handler(self, path, args):
        print(path)
        for x in range(self.matrix.cols):
            for y in range(self.matrix.rows):
                self.matrix.set_led(x, y)

    def update(self, timeout=10):
        self.recv(timeout)
        self.matrix.root.after(100, self.update)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        socket = '/tmp/testhat.socket'
    else:
        socket = sys.argv[1]

    mh = MonoHorn(socket)
