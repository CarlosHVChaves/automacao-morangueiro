from flask import Flask, Response, request
from random import *
import numpy as np
from consulta_clima import consulta
from classification import classificacao

app = Flask(__name__)


@app.route('/random_number', methods=['GET'])
def random_number():
    number = round(np.random.uniform(0.0, 10.0), ndigits=1)
    return str(number)


@app.route('/consulta_clima', methods=['GET'])
def consulta_clima():
    resultado = consulta()
    return resultado


@app.route('/agente_inteligente', methods=['GET'])
def agente_inteligente():
    temperatura_ambiente = request.args.get('temperatura_ambiente', None)
    umidade_ambiente = request.args.get('umidade_ambiente', None)
    status_umidade_solo = request.args.get('status_umidade_solo', None)
    print (temperatura_ambiente, umidade_ambiente, status_umidade_solo)
    agente = classificacao(temperatura_ambiente, umidade_ambiente, status_umidade_solo)
    return str(agente)


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)
