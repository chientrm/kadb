# kadb

Lightning fast Immutable Key-Array Database.

# Common use cases

- Logging server
- Uneditable chat room/forum
- Mailbox

# Why `kadb`?

- Write value: O(logn)
- Read subarray: O(logn)

_n is the total number of key_

## Implementation

- Linux's io_uring
- AVL Tree
- Array accumulated items length
- Single-threaded

## Build

### Requirements

- Python 3
- Build essential

### Clone project

```bash
git clone https://github.com/chientrm/kadb
```

### Build and install liburing

```bash
cd liburing
./configure
make
sudo make install
cd ..
```

### Configure

```bash
python configure.py
```

### Build release

```bash
make
```

## Start server

```bash
dist/release/kadb
```

Output: `Listening on http://localhost:8080`

## Test correctness

Run test while server is running

```bash
pip install pytest
python -m pytest
```

# How to use?

## Put value

URI format: `/put/<metadata_length>/<key_length>/<value_length>/<key><value>`

#### Example

1. `key_length = 2^32 = 4294967296`
2. `value_length = 2^64 = 18446744073709600000`
3. `metadata_length = strlen(key_length) + strlen(value_length) = 10 + 20 = 30`

Command: `curl http://localhost:8080/put/030/4294967296/18446744073709600000/<key><value>`

Result: `Code 204`

_`metadata_length` is always 3 bytes filled with leading `0`_

## Get values

URI format: `/get/<metadata_length>/<key_length>/<from>/<count>/<key>`

#### Example

1. `key_length = 2^32 = 4294967296`
2. `from = 2^32 = 4294967296`
3. `count = 2^64 = 18446744073709600000`
4. `metadata_length = strlen(key_length) + strlen(from) + strlen(count) = 10 + 10 + 20 = 40`

Command: `curl http://localhost:8080/get/040/4294967296/4294967296/18446744073709600000/<key> -o result.bin`

The result header contains

```
Kadb-count: 10
Kadb-found: 2
```

meaning the key has total 10 values and 2 values is found in range `[from:from+count]`.

`result.bin`:

- First 8 bytes (Little Endian): length of the first value (Ex: `2`).
- Next 8 bytes (Little Endian): length of the first value + length of the second value (Ex: `2 + 6 = 8`).
- Next 2 bytes: first value
- Next 6 bytes: second value

### Why not put metadata in HTTP headers?

- HTTP headers order are not guaranteed to be retained on receiving because of network nodes interference. Thus, if you want to get one header, you still need to read the whole headers block, split them by `\r\n`, parse them and select the requested values. Furthermore, the headers size are non-determined till the appearance of `\r\n\r\n`.
- By composing metadata directly into the uri, we can ignore parsing the headers and body.

## Benchmarks

## FAQ

- What is the size limit of key or value?

  Theoretically 2^64 bytes. As long as your device can hold.

- Is `kadb` memory efficient?

  `kadb` is specifically designed to be memory efficient. Query data are mapping directly to and from the AVL tree without modification.

- Does `kadb` support HTTPS, content compression or authentication?

  `kadb` always response with `HTTP/1.0` without content compression or authentication support. Please use reverse proxy like `nginx` to wrap `kadb`.

- Does `kadb` has built-in ingress or egress control?

  To focus on the core features, `kadb` support neither ingress or egress control.
