# EFT_Scratch

## Build & Run
Run cmake to build the library and runtime code. The execution file will be made under build/.
```bash
cmake -S . -B build -DEFT_BUILD_TESTS=ON
cmake --build build -j
./build/gausselm_eft
```

## Repository Structure

```
.
├── include/
│   └── eft/
│       ├── eft.hpp
│       ├── shadow_memory.hpp
│       ├── tmp_mta_space.hpp
│       └── shadow_stack.hpp # implementing
├── src/
│   └── eft/
│       ├── shadow_memory.cpp
│       ├── tmp_mta_space.cpp
│       └── shadow_stack.cpp # implementing
└── tests/
    ├── gausselm_.c
    └── gausselm_eft.cpp
```

## `fp_entry`

```cpp
struct fp_entry {
    double error;       // computed FP value from program
    double value;       // propagated rounding error
    fp_op opcode;       // ADD, MUL, LOAD, STORE, ...
    size_t linenum;     // line number from source code
    fp_entry* lhs;      // left operand of operator
    fp_entry* rhs;      // right operand of operator
    size_t timestamp;   // incremented timestamp for each entry
    size_t static_id;   // static instruction id (hash of file:line:function)
};
```
* `timestamp` is increased when a space is allocated in the temporary metadata space.
* `lhs`/`rhs` are `nullptr` or a pointer to valid entry in a temporary metadata space. Entries are valid when the timestamp of an entry in the temporary metadata space and the timestamp in the last writer map is equal.

## ShadowMemory

* `map<uintptr_t, fp_entry>` keyed by `reinterpret_cast<uintptr_t>(addr)`.
* Methods:
  * `fp_entry* peek(addr)` → returns the value of the addr. it doesn't exist, return `nullptr`
  * `fp_entry& on_store(dest, org)` → adds the fp_entry to `map<uintptr_t, fp_entry>`

## Temporary Metadata Space

* Circular queue storing `fp_entry` nodes created by EFT ops and loads.
* `alloc()` returns the address of the next slot and increments the global timestamp.
* Calculate the propagated rounding error using error free transformation.
* Arithmetic Operations checks the validity of operands and calculates both the operation and the propagated rounded error of the operation.
  * `t_add(a, b, site_id, linenum)`
  * `t_sub(a, b, site_id, linenum)`
  * `t_mul(a, b, site_id, linenum)`
  * `t_div(a, b, site_id, linenum)`
* EFT kernels:
  * `t_const(program_value, site_id, linenum)` initiate a fp_entry and save the value.
  * `void PropSumError(a, da, b, db, x, dx)` calculates the sum of two fp_entry and updates the error to dx
  * `void PropProdError(a, da, b, db, x, dx)` calculates the production of two fp_entry and updates the error to dx
  * `void PropDivError(a, da, b, db, x, dx)`calculates the division of two fp_entry and updates the error to dx
  * subtraction uses `PropSumError` with signs reverted
* Selective Shadow Execution: Given a fp_entry, it will backtrack the operands used for operations. 
  * `void backtrack(x, ind)` follows the `lhs` and `rhs` of a fp_entry to facilitate debugging. Validate `lhs` and `rhs` are entries with lower timestamps. `vector<fp_entry*> tracing` will hold the values.

## Last‑Writer Map (LWM)

* `map<size_t, lwrm_value>`.
* Used to validate freshness and avoid stale edges.
