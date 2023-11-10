from pynput.keyboard import Key, Controller
import keyboard
import time

def handleKeyPress(eventDetails):
    print(eventDetails)

if __name__ == '__main__':
    kbd = Controller()
    kbd.pressed('m', handleKeyPress)

    while True:
        time.sleep(3)
        print('trying to press')
        kbd.press('m')
        # print(ConnectToESP('10.0.0.94', 1234, 5))