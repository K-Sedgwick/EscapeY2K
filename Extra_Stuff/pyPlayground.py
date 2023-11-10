from pynput.keyboard import Key, Controller, Listener
import keyboard

import threading
import time

def on_press(key, boolrando):
    print('{0} pressed'.format(key))

if __name__ == '__main__':
    keyboard.add_hotkey('d', on_press, args = ('d', True))
    keyboard.add_hotkey('g', on_press, args = ('g', False))
    keyboard.add_hotkey('m', on_press, args = ('m', True))
    keyboard.add_hotkey('f', on_press, args = ('f', False))
    keyboard.add_hotkey('e', on_press, args = ('e', False))

    while (not keyboard.is_pressed('esc')):
        time.sleep(3)
        print("stuff")