#!/bin/bash

cat benchmark_urls_post.txt | parallel 'ab -n 100 -c 10 -m POST {}' > benchmark_post_result.txt
cat benchmark_urls_get.txt | parallel 'ab -n 100 -c 10 -m GET {}' > benchmark_get_result.txt
