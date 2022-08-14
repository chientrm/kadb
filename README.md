# kadb

Lightning fast Immutable Key-Array Database.

```
Warning: `kadb` is still experimental. There would be unexpected breaking changes.
```

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

```
curl -X PUT http://localhost:8080/key/value
```

Result: `Code 204`

## Get values

```
curl http://localhost:8080/key/from/count -o result.bin
```

The result headers contain

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
