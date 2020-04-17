import serial
from struct import unpack, pack


class ComPort(serial.Serial):
    """ Objet permettant de communiquer avec un port COM """
    
    def __init__(self, port='COM3', baudrate='115200'):
        """ constructeur de la classe, initialise les champs """
        serial.Serial.__init__(self)
        self.baudrate = baudrate     # le debit est de 115200 bits
        self.rtscts = True           # on utilise le handshake hardware du port serie
        self.timeout = 0.1           # Les methodes read et write auront un time out de 0.1s
        self.port = port             # Designe le port COM a ouvrir
        
    def getInt(self):
        "Essaye de lire un 'int16' emis par le PRC. Abandonne apres le ""time out"" defini (1s par defaut)"
        tmp = self.read(2)       # lit 2 octets (tmp de type 'Bytes')
        tmp = unpack('h',tmp)               # convertit le 'Bytes' en l'entier correspondant (tmp devient un tuple de int)
        tmp = tmp[0]                        # extrait l'entier du tuple (tmp devient un int)
        return tmp

    def getFloat(self):
        """ essaye de lire un 'float32' emis par le PRC. Abandonne apres le ""time out"" defini """
        tmp = self.read(4)          # lit 4 octets (tmp de type 'Bytes')
        tmp = unpack('f',tmp)       # convertit le 'Bytes' en le flottant correspondant (tmp devient un tuple de float)
        tmp = tmp[0]                # extrait le flottant du tuple (tmp devient un float)
        return tmp

    def sendInt(self, data):
        """ essaye d'envoyer un 'int16' au PRC. Abandonne apres le ""time out"" defini """
        tmp = int(data)             # transforme la donnee en entier (au cas ou)
        tmp = pack('h', tmp)        # transforme la donnee en un 'Bytes'
        self.write(tmp)             # ecrit la donnee sur le port serie
    
    def sendFloat(self, data):
        """ essaye d'envoyer un 'float' au PRC. Abandonne apres le ""time out"" defini """
        tmp = pack('f', data)       # on transforme la donnee en un 'Bytes'
        self.write(tmp)             # on l'ecrit sur le port serie
