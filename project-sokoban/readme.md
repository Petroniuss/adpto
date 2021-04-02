# Sokoban

### How to run the solver?

- make all TEST=[NAME_OF_THE_TEST]

### How to measure time?

- make 
- time ./build/brute_force < tests/misan.in


## Checkpoint 1 - Pure bruteforce with caching previously visited states
tested on `misan.in`
- bfs - `40.17s user 0.60s system 98% cpu 41.341 total` - < 1.8 GB
- dfs - `3.97s user 0.36s system 96% cpu 25.078 total`  - < 1.0 GB