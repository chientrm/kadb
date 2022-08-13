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
- realloc strategy `new memsize = old memsize * 2`
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

## Try with `curl`

### Write value

URI format: `/<header_length>/<key_length>/<value_length>/<key>/<value>`

#### Example 1

1. `key_length = 2^32 = 4294967296`
2. `value_length = 2^64 = 18446744073709600000`
3. `header_length = strlen(key_length) + strlen(value_length) = 10 + 20 = 30`

Command: `curl -X POST http://localhost:8080/030/4294967296/18446744073709600000/<key>/<value>`

#### Example 2

1. `key_length = 1 = 1`
2. `value_length = 2 = 2`
3. `header_length = strlen(key_length) + strlen(value_length) = 1 + 2 = 3`

Command: `curl -X POST http://localhost:8080/003/1/2/<key>/<value>`

### Read subarray

URI format: `/<header_length>/<key_length>/<from>/<count>/<key>`

#### Example 1

1.  Let `key_length = 2^32 = 4294967296`
2. `from = 2^32 = 4294967296`
3. `count = 2^64 = 18446744073709600000`
4. `header_length = strlen(key_length) + strlen(from) + strlen(count) = 10 + 10 + 20 = 40`

Command: `curl http://localhost:8080/040/4294967296/4294967296/18446744073709600000/key`

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

### Why not just use HTTP headers?

- HTTP headers order are not guaranteed to be retained on receiving because of network nodes interference. Thus, if you want to get one header, you still need to read the whole headers block, split them by `\r\n`, parse them and select the requested values. Furthermore, the headers size are non-determined until the first appearance of `\r\n\r\n`.
- By composing metadata directly into the uri, we can ignore parsing the headers and body.

## Build debug

```bash
make -f Makefile.debug
```

## Benchmarks
