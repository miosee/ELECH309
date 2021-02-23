# -*- coding: cp1252 -*-
import sys                      # pour sys.exit
import serial                   # package de gestion du port serie
from matplotlib import pyplot   # pour l'affichage


# Configuration du port serie
serialPort = serial.Serial()    # creation de l'objet associe au port serie
serialPort.baudrate = 57600     # definition du baudrate
serialPort.port = 'COM6'        # selection du port serie
#serialPort.timeout = 0.001

# Connexion au dsPIC
try:
    serialPort.open()               # on tente d'etablir la connexion
except:
    print('Echec de la connexion')  # En cas de probleme,
    sys.exit()                      # on ferme l'application

# Parametres de l'acquisition
sampleNb = 1000                     # Nombre d'echantillons a lire
samplePer = 200E-6                  # Periode d'echantillonnage

# Creation d'un vecteur pour stocker les donnees
#data = numpy.zeros((sampleNb,1))
data = []
time = []

# Recuperation des donnees
serialPort.write(b's')              # Envoi du caractere de synchro
for a in range(sampleNb):
    tmp1 = serialPort.read(1)       # on lit 1 octet sur le port serie
    tmp2 = ord(tmp1)
    data.append(3.3*tmp2/255)       # on calcule la tension correspondante, en V
    time.append(a*samplePer)

# Deconnexion du dsPIC
serialPort.close()

# Affichage des donnees
endTime = sampleNb*samplePer                # Duree de l'acquisition, en s
pyplot.plot(time, data)
pyplot.grid()
pyplot.xlabel('t (s)')
pyplot.ylabel('CH1 (V)')
pyplot.show()
