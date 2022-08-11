# kadb

Ultra fast Immutable Key-Array Database.

# Features

- Post new value: O(logn)
- Read subarray: O(logn)

_n is the total number of key_

## Algorithms used

- io_uring
- AVL Tree
- Array offsets
- Dynamic memory allocation where `new memsize = old memsize * 2 + 1`

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
