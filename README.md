# kadb

Lightning fast Immutable Key-Array Database.

# Common use cases

- Logging server
- Uneditable chat room/forum
- Uneditable mailbox

# Why `kadb`?

- Write value: O(logn)
- Read subarray: O(logn)

_n is the total number of key_

## Implementation

- Linux's io_uring
- AVL Tree
- Array accumulated items length
- realloc strategy `double`
- Single-threaded

## Clone
```bash
git clone https://github.com/chientrm/kadb
```

## Build

```bash
cd kadb
./configure
make kadb
```

## Start server

```bash
kadb
```

`Listening on http://localhost:8080`

## GET

Get `10` value of key `key-1` (5 bytes length) from offset `0`

```bash
curl http://localhost:8080/0005/0000/0010/key-1
```

Example result

```
HTTP Header
------------------------
Kadb-n_items: 2
Content-Type: text/plain
Content-Length: 13

HTTP Body
------------------------
value1;value2;
```

meaning key `key-1` has total 2 items.

## PUT

Put value `1234` to key `abc`.
```bash
curl http://localhost:8080/0003/0004/abc1234
```

Result status code `200`

## Benchmarks

...coming soon...
