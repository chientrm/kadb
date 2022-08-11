# kadb

Lightning fast Immutable Key-Array Database.

# Common use cases

- Logging server
- Uneditable chat room/forum

# Why `kadb`?

- Post new value: O(logn)
- Read subarray: O(logn)

_n is the total number of key_

## Implementation

- Linux's io_uring
- AVL Tree
- Array offsets
- realloc strategy `new memsize = old memsize * 2`
- HTTP request max size = 1024 bytes
- Unlimited response `application/octet-stream`

## Build

### Requirements

- Python 3
- Build essential

Clone project

```bash
git clone https://github.com/chientrm/kadb
```

Configure

```bash
python configure.py
```

Build release

```bash
make
```

## Start server

```bash
dist/release/kadb
```

## Test correctness

Run test while server is running

```bash
pytest
```

## Debug

```bash
make -f Makefile.debug
```

## Benchmarks
