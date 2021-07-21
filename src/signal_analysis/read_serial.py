import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation




def shift5(arr, num, fill_value=np.nan):
    result = np.empty_like(arr)
    if num > 0:
        result[:num] = fill_value
        result[num:] = arr[:-num]
    elif num < 0:
        result[num:] = fill_value
        result[:num] = arr[-num:]
    else:
        result[:] = arr
    return result

N = 400;

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
	print(res)
	
	"""if(res == '\r'):#caso a ultima string recebida seja essa
		#if(aux == N): #
		#	aux1.clear()
		#	aux1.plot(array_msg)
		#	fig.canvas.draw()
		#	aux = 0;
    			
		temp = float(msg[::-1])
		array_msg = shift5(array_msg, -1)
		array_msg[-1] = temp
		aux1.clear()
		aux1.plot(array_msg)
		fig.canvas.draw()

		#aux = aux + 1#incrementar variavel auxiliar 
		print(temp)
		msg = ""
	msg = res + msg#adiciona caractere recebido a string mensagem
			
"""
