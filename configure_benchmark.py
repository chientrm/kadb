import random
random.seed(2020)

MAX_KEY_LENGTH = 16
MAX_VALUE_LENGTH = 990

CHARSETS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


def randst(len):
    return ''.join(random.choice(CHARSETS) for _ in range(len))


n_keys = 1000
n_items = 1000
keys = [randst(MAX_KEY_LENGTH) for _ in range(n_keys)]
values = [randst(MAX_VALUE_LENGTH) for _ in range(n_keys)]


def generate_urls():
    post_urls = [
        f'http://localhost:8080/{key}/{value}' for key, value in zip(keys, values)]
    with open('benchmark_urls_post.txt', 'w') as f:
        f.write('\n'.join(post_urls))
    get_urls = [f'http://localhost:8080/{key}/0/{n_items}' for key in keys]
    with open('benchmark_urls_get.txt', 'w') as f:
        f.write('\n'.join(get_urls))


if __name__ == '__main__':
    generate_urls()
