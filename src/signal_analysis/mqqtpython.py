import paho.mqtt.client as mqtt
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import timeit
import pandas as pd

N = 100 #tamanho do array plot
client_get = mqtt.Client(client_id='', protocol=mqtt.MQTTv31)
client_get.connect("broker.hivemq.com", 1883)
array_msg = np.array([0] * N, dtype=float);#string que salvara 

def save(user, dado):#adicionando dados a um arquivo csv
    dado = np.array([dado])
    dado = pd.DataFrame(dado)
    dado.to_csv (r'../../contents/'+user+'.csv', mode='a',  index = False, header= False, encoding='utf-8')
        
def gaussian(u, sigma, f):#funcao gaussiana no ponto
    return 1/(sigma*np.sqrt(2*np.pi))*np.exp(-1/2*pow((f-u)/sigma, 2))

def classificar(f):#funcao para classificar resultado
    df = pd.read_csv("../../contents/classificacao.csv")
    df = df.values
    if(f == 0):
        print("aparelho desconectado")
    elif(gaussian(df[0,0], df[0,1], f) > gaussian(df[1,0], df[1,1], f)):
        print("freq normal")
    else:
        print("freq do batimento cardiaco em situacao de risco")
        client_get.publish("amostrar/ans","L")

def shift5(arr, num, fill_value=np.nan):#
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
def callback(client, userdata, message):
    global user
    msg_string = str(message.payload.decode("utf-8"));#recebe string 
    if(msg_string[0] == 'p'):#mensagem de identificacao do usuario conectado
        user = msg_string
        print(user)
    else:# mensagem contando a informacao
        dado = float(msg_string)
        print(dado)
        classificar(dado)
        save(user, dado)

if __name__ == "__main__":
	topic =  "amostrar/heart_beat"  #define topico de subscribe
	client_get.on_message = callback #define que ficaremos sempre escutando o que estiver chegando
	client_get.subscribe(topic, qos=0)#cria o subscribe
	try:
		client_get.loop_forever() #escutando inifitamente
	except KeyboardInterrupt:#caso o teclado interrompa, fechar conexao
		client_get.disconnect()#conexao fechada