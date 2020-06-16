# Comparison of Weld vs. state-of-the-art query paradigms

This repository is based on https://github.com/TimoKersten/db-engine-paradigms.

## Building

Download `sandbox.sh` and modify `BASE` (base prefix to use) as well as `N` (number of threads).

## Notes
### Q1
* Already did this one (partially) as part of [1]
* A generalized group-by can be implemented using a `dictmerger` [2]

### Q3
 * Joins can be implemented using 
 * As Weld does not support strings, we need an UDF for string equality
 * TimoKersten's string implementation requires us to store the length too i.e. a string is a tuple `{i16, i64}`

### Q6
 * Part of Weld benchmarks [3]

### Q9
 * GroupBy on string key, not clear how to do that

## References
* [1] Tim Gubner and Peter Boncz, Exploring Query Execution Strategies for JIT, Vectorization and SIMD, ADMS 2017, https://t1mm3.github.io/assets/papers/adms17.pdf
* [2] Mihai Varga. Just-in-time Compilation in MonetDB with Weld. Vrije Universiteit Amsterdam, 2018, https://homepages.cwi.nl/~boncz/msc/2018-MihaiVarga.pdf
* [3] https://github.com/weld-project/weld-benchmarks