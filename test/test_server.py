from random import random
import requests
import random
import base64
import math

MAX_ENCODED_KEY_LENGTH = 16
MAX_KEY_LENGTH = math.floor(MAX_ENCODED_KEY_LENGTH * 5 / 8)
MAX_ENCODED_VALUED_LENGTH = 1024 - len("POST /0123456789ABCDEF/ HTTP/1.1") - 1
MAX_VALUE_LENGTH = math.floor(MAX_ENCODED_VALUED_LENGTH * 5 / 8) * 10


def random_key(len):
    raw_key = bytes(random.getrandbits(8) for _ in range(len))
    return base64.b32encode(raw_key).decode('ascii')


def random_value(len):
    raw_value = bytes(random.getrandbits(8) for _ in range(len))
    return base64.b32encode(raw_value).decode('ascii')


def parse_response(count, content):
    result = []
    for i in range(int(count)):
        result.append(int.from_bytes(content[i*8:(i+1)*8], byteorder='little'))
    return result


def test_write():
    key = random_key(MAX_KEY_LENGTH)
    value = random_value(MAX_VALUE_LENGTH)
    print([key, value])
    response = requests.post(f'http://localhost:8080/{key}/{value}')
    assert response.status_code == 204
    response = requests.get(f'http://localhost:8080/{key}/0/10')
    assert response.status_code == 200
    count = response.headers['Kadb-count']
    result = parse_response(count, response.content)
    print(result)
