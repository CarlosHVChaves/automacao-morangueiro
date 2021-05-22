# from sklearn.utils import murmurhash3_32
from sklearn.tree import DecisionTreeClassifier
from sklearn.preprocessing import LabelEncoder
import pandas as pd
from consulta_clima import consulta


def classificacao(temperatura_ambiente, umidade_ambiente, status_solo):
        # classificaçao
        df_classification = pd.read_excel("classification.xlsx")
        le_classification = LabelEncoder()
        df_classification['STATUS SOLO NUMBER'] = le_classification.fit_transform(
            df_classification['STATUS SOLO'])
        x_classification = df_classification[[
            'TEMPERATURA AMBIENTE', 'UMIDADE AMBIENTE', 'STATUS SOLO NUMBER']]
        y_classification = df_classification['BOMBA']
        # teste
        df_teste = pd.read_excel("test_classification.xlsx")
        le_test = LabelEncoder()
        df_teste['STATUS SOLO NUMBER'] = le_test.fit_transform(
            df_teste['STATUS SOLO'])
        x_test = df_teste[['TEMPERATURA AMBIENTE',
                           'UMIDADE AMBIENTE', 'STATUS SOLO NUMBER']]

        clf = DecisionTreeClassifier(random_state=42)
        # ensinando
        clf = clf.fit(x_classification, y_classification)
        # testando
        # y_pred = clf.predict(x_test)
        # # print(y_pred)

        resultado = clf.predict(
            [[temperatura_ambiente, umidade_ambiente, status_solo]])
        print(resultado)
        return resultado


if __name__ == '__main__':
    classificacao(0, 0, 1)
