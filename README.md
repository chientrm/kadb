# kadb

Lightning fast Immutable Key-Array Database.

# Common use cases

- Logging server
- Uneditable chat room/forum

# Why `kadb`?

- Write value: O(logn)
- Read subarray: O(logn)

_n is the total number of key_

## Implementation

- Linux's io_uring
- AVL Tree
- Array accumulated items length
- realloc strategy `new memsize = old memsize * 2`
- HTTP request max size = 1024 bytes
- Unlimited key length, value length, http response `content-length`
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
pip install -g pytest
pytest
```

## Test manually

### Write value

```bash
curl -X POST http://localhost:8080/key-1/value1
curl -X POST http://localhost:8080/key-1/v2
curl -X POST http://localhost:8080/key-2/value3
```

### Read subarray

Read 10 value of key `key-1` from index `0`

```bash
curl http://localhost:8080/key-1/0/10 -o result.bin
```

The result header contains

```
Kadb-count: 2
```

meaning 2 values returned.

`result.bin`:

- First 8 bytes: `6` (Little Endian), length of the first value.
- The next 8 bytes: `8` (Little Endian), `8` - `6` = `2` = length of the second value.
- The next 6 bytes: `value1`
- The final 2 bytes: `v2`

## Build debug

```bash
make -f Makefile.debug
```

## Benchmarks
