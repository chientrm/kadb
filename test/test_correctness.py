from random import random
import requests
import random
import os
from urllib.parse import quote

random.seed(1010)


def randst(len):
    return quote(os.urandom(len))


def parse_response(found, content):
    accs = []
    items = []
    for i in range(int(found)):
        accs.append(int.from_bytes(content[i*8:(i+1)*8], byteorder='little'))
    content = content[int(found)*8:]
    for i in range(int(found)):
        start = accs[i] - accs[0]
        end = accs[i+1] - accs[0] if i+1 < int(found) else len(content)
        items.append(content[start:end].decode('ascii'))
    return items


def put(key, value):
    print(key)
    res = requests.put(
        f'http://localhost:8080/{len(key):4}{len(value):4}{key}{value}')
    assert res.status_code == 204


def get(key, values):
    _from, _count = 0, len(values)
    response = requests.get( 
        f'http://localhost:8080/{len(key):4}{_from:4}{_count:4}{key}')
    count = response.headers['Kadb-count']
    found = response.headers['Kadb-found']
    _values = parse_response(found, response.content)
    assert count == _count
    assert found == count
    assert(str(_values) == str(values))


def correctness(n_keys, n_values, key_length, value_length):
    keys = [randst(key_length) for _ in range(n_keys)]
    values = [randst(value_length) for _ in range(n_values)]
    [put(key, value) for key in keys for value in values]
    get(keys, values)


def test_correctness():
    correctness(10, 10, 1024, 1024)
