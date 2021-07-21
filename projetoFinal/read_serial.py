import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

N = 1000;

s = serial.Serial('COM3', baudrate = 115200, timeout = 1) #criando porta serial


#etapa para plotar dados recebidos
fig = plt.figure()
aux1 = fig.add_subplot(111)
plt.ion()
fig.show()
fig.canvas.draw()


#definindo variaveis utilizadas
msg = "" #variavel do tipo string onde ficara salva a mensagem recebida
array_msg = np.array([0] * N, dtype=float);#string que salvara 
encoding = 'utf-8'
aux = 0; #variavel auxiliar que incrementa 


while(1):# repetir sempre	
	res = s.read()#coletar byte da porta serial
	res = str(res, encoding)#converter byte em string
	if(res == '\r'):#caso a ultima string recebida seja essa
		if(aux == N): #
			aux1.clear()
			aux1.plot(array_msg)
			fig.canvas.draw()
			aux = 0;
    			
		temp = float(msg[::-1])
		array_msg[aux] = temp
		aux = aux + 1#incrementar variavel auxiliar 
		#print(array_msg)
		msg = ""
	msg = res + msg#adiciona caractere recebido a string mensagem
			

