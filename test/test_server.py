import requests


def test_write():
    r = requests.post('http://localhost:8080/test/test')
    assert r.status_code == 200
    r = requests.get('http://localhost:8080/test/0/1')
    assert r.status_code == 200
    assert r.text == 'test'
