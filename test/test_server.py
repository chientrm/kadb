from random import random
import requests
import random
random.seed(10)

MAX_KEY_LENGTH = 16
MAX_VALUE_LENGTH = 990

CHARSETS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


def randst(len):
    return ''.join(random.choice(CHARSETS) for _ in range(len))


n_keys = 10
n_items = 10
keys = [randst(MAX_KEY_LENGTH) for _ in range(n_keys)]
values = [[randst(MAX_VALUE_LENGTH) for _ in range(n_items)]
          for _ in range(n_keys)]


def parse_response(count, content):
    accs = []
    items = []
    for i in range(int(count)):
        accs.append(int.from_bytes(content[i*8:(i+1)*8], byteorder='little'))
    content = content[int(count)*8:]
    for i in range(int(count)):
        start = accs[i] - accs[0]
        end = accs[i+1] - accs[0] if i+1 < int(count) else len(content)
        items.append(content[start:end].decode('ascii'))
    return items


def write():
    for i in range(n_items):
        for j in range(n_keys):
            response = requests.post(
                f'http://localhost:8080/{keys[j]}/{values[j][i]}')
            assert response.status_code == 204


def read_and_assert():
    for j in range(n_keys):
        response = requests.get(f'http://localhost:8080/{keys[j]}/0/{n_items}')
        assert response.status_code == 200
        count = response.headers['Kadb-count']
        items = parse_response(count, response.content)
        assert all(item == values[j][i] for i, item in enumerate(items))


def test_correctness():
    # write()
    read_and_assert()
