import pyfirmata
import time


class bomba:
    def setup(self):
        pass

    def __init__(self):
        self.board = pyfirmata.Arduino('COM3')
        self.tempo = 0
        self.porta = 0

    def ligar_bomba(self, tempo, porta):
        self.board.digital[porta].write(1)
        time.sleep(tempo)
        self.board.digital[porta].write(0)


if __name__ == '__main__':
    bomba = bomba()
    print('Bomba ligada')
    bomba.ligar_bomba(7, 12)
    print('Bomba desligada')
