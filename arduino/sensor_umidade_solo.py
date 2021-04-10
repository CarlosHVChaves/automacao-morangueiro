import pyfirmata


class umidade_solo:
    def setup(self):
        pass

    def __init__(self):
        self.board = pyfirmata.Arduino('COM3')
        self.iterator = pyfirmata.util.Iterator(self.board)
        self.porta = 0
        self.umidade = 0

    def ler_umidade_solo(self, porta):
        self.iterator.start()
        self.board.analog[porta].enable_reporting()
        while self.umidade is None or self.umidade is 0:
            self.umidade = self.board.analog[porta].read()
        self.umidade = self.umidade * 1000
        porcentagem = int(100 - ((100 * self.umidade) / 1000))
        if 0 < self.umidade < 400:
            print(f'A umidade está em {porcentagem} %. O solo está umido')
        elif 400 < self.umidade < 800:
            print(f'A umidade está em {porcentagem} %. O solo está com umidade moderada')
        elif 800 < self.umidade < 1000:
            print(f'A umidade está em {porcentagem} %. O solo está seco')


if __name__ == '__main__':
    umidade_solo = umidade_solo()
    print('Medindo umidade')
    umidade_solo.ler_umidade_solo(1)
    print('Medição compeltada')
