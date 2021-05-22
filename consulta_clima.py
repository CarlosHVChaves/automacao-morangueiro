import requests
import json

iTOKEN = "dd7b98fe697790d9169752c2b9b15a00"
iCIDADE = "3477"


def consulta():
    iURL = "http://apiadvisor.climatempo.com.br/api/v1/weather/locale/" + \
        iCIDADE + "/current?token=" + iTOKEN
    iRESPONSE = requests.request("GET", iURL)
    iRETORNO_REQ = json.loads(iRESPONSE.text)
    iCONDITION = iRETORNO_REQ['data']['condition']
    # print(iCONDITION)
    # print(type(iCONDITION))
    # if "nuvens" in iCONDITION:
    #     print("Deu certo")
    # else:
    #     print("Nao deu certo")
    return iCONDITION


if __name__ == '__main__':
    consulta()
