from random import random
import requests
import random
import os
random.seed(1010)


def randst(len):
    return os.urandom(len)


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


def put(keys, values):
    for key in keys:
        for value in values:
            key_len = len(key)
            value_len = len(value)
            response = requests.put(
                f'http://localhost:8080/{key_len}{value_len}{key}{value}')
            assert response.status_code == 204


def get(keys, values):
    _from, _count = 0, len(values)
    for key in keys:
        key_len = len(key)
        response = requests.get(
            f'http://localhost:8080/{key_len}{_from}{_count / 2}{key}')
        count = response.headers['Kadb-count']
        found = response.headers['Kadb-found']
        items = parse_response(found, response.content)
        assert count == _count
        assert found == count / 2
        assert(str(items) == str(values))


def correctness(n_keys, n_values, key_length, value_length):
    keys = [randst(key_length) for _ in range(n_keys)]
    values = [randst(value_length) for _ in range(n_values)]
    put(keys, values)
    # get(keys, values)


def test_correctness():
    correctness(1024, 1024, 1024, 1024)
