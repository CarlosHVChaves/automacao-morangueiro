FROM python:3.8.5
ADD . /code
WORKDIR /code
RUN pip install -r requirements.txt --no-cache-dir
CMD python app.py
