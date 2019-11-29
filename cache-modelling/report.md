The following caches are modelled for this project:
1. an instruction cache, direct-mapped cache with 32KB capacity and 64-byte blocks.
2. an instruction cache,  4-way set associative cache with 32KB capacity and 32-byte blocks and LRU replacement.
3. a data cache, 8-way set-associative data cache with 32KB capacity and 64-byte blocks.
4. a modified cache with next block prefetch

The victim cache was not able to be implemented. After modeling the caches, we can measure the performance in the 4 benchmarks.

FPPPP
In the fpppp benchmark, a direct-mapped cache with 32KB capacity and 64-byte blocks seems to perform better than a 4-way set associative cache with 32KB capacity and 32-byte blocks and LRU replacement. It has less miss rate than the set associative cache with 5.86% and 11.06% respectively, which is not a huge difference.
The 8-way set-associative data cache with 32KB capacity and 64-byte blocks has a load miss rate of 4.520% and a store miss rate of 4.196%. Pretty similar in value. The writeback ration is 0.4821%.
After adding a prefetch, we can see a prefetch miss rate of 0.9995 and prefetch success rate of 0.0005. Meaning the prefetch does not help that much in this benchmark.

GCC
In the gcc benchmark, a 4-way set associative cache with 32KB capacity and 32-byte blocks and LRU replacement seems to perform better than a direct-mapped cache with 32KB capacity and 64-byte blocks. It has less miss rate than the set associative cache with 3.77% and 3.412% respectively. Although the difference is less, the same conclusion can be reached.
The 8-way set-associative data cache with 32KB capacity and 64-byte blocks has a load miss rate of 1.8485% and a store miss rate of 0.7676%. The writeback ration is 1.2336%.
After adding a prefetch, we can see a prefetch miss rate of 0.95879 and prefetch success rate of 0.0412. The success rate performs better than other benchmark, however it still does not help too much.

GO
In the go benchmark, a direct-mapped cache with 32KB capacity and 64-byte blocks seems to perform just as great as a 4-way set associative cache with 32KB capacity and 32-byte blocks and LRU replacement. Both has miss rate of 2.56% and 2.9877% respectively. The difference is quite similar to gcc benchmark.
The 8-way set-associative data cache with 32KB capacity and 64-byte blocks has a load miss rate of 1.6954% and a store miss rate of 0.8256%, which are very small. The writeback ration is 2.5222%.
After adding a prefetch, we can see a prefetch miss rate of 0.9817 and prefetch success rate of 0.01827. The miss rate and success rate performs are again similar to gcc.

VPR
In the vpr benchmark, a direct-mapped cache with 32KB capacity and 64-byte blocks seems to perform worse than a 4-way set associative cache with 32KB capacity and 32-byte blocks and LRU replacement as well. It has more miss rate than the set associative cache with 0.56% and 0.0253% respectively. The set associative cache seems to work very well for this benchmark.
The 8-way set-associative data cache with 32KB capacity and 64-byte blocks has a load miss rate of 2.6149% and a store miss rate of 0.7402%. The writeback ration is 2.5297%.
After adding a prefetch, we can see a prefetch miss rate of 0.9626 and prefetch success rate of 0.037397. The miss rate and success rate performs are again similar to gcc and go.
