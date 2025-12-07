# Line Drawing Algorithm Comparison

A performance comparison between two line drawing algorithms, featuring a tribute to the legendary Terry A. Davis and his innovative work in TempleOS.

## About

This program compares two line drawing algorithms:

1. **TAD 8.8 Fixed-Point** - A branchless line drawing algorithm inspired by Terry A. Davis's work in TempleOS, using 8.8 fixed-point arithmetic for smooth interpolation
2. **Bresenham Algorithm** - The classic integer-only line drawing algorithm

## Tribute to Terry A. Davis

This implementation pays homage to **Terry A. Davis** (1969-2018), the brilliant programmer who single-handedly created TempleOS - a complete operating system written from scratch. Terry's innovative approaches to graphics programming, including his elegant line drawing techniques using fixed-point arithmetic, continue to inspire programmers today.

Terry's work on TempleOS demonstrated that beautiful, efficient code could emerge from unconventional thinking. His branchless algorithms and mathematical insights into computer graphics remain influential in the programming community.

*"God's third temple is a computer." - Terry A. Davis*

## Features

- Visual ASCII art output showing both algorithms side-by-side
- Performance benchmarking with microsecond precision
- Multiple test cases including horizontal, vertical, diagonal, and arbitrary lines
- Correctness verification between algorithms

## Compilation

### macOS/Linux with GCC or Clang
```bash
g++ -std=gnu++17 -O3 -march=native line_demo.cpp -o line_demo
```

### Alternative compilation options
```bash
# With clang
clang++ -std=c++17 -O3 -march=native line_demo.cpp -o line_demo

# Debug build
g++ -std=gnu++17 -g -O0 line_demo.cpp -o line_demo
```

## Usage

Simply run the compiled executable:
```bash
./line_demo
```

The program will display:
- Visual representation of each line drawn by both algorithms
- Performance timing for single runs
- Batch performance testing with 10,000 iterations per algorithm
- Correctness comparison between the two methods

## Sample Output

```
=== Test: shallow +slope  (2,2)->(45,5) ===
Single-run:  TAD 8.8 = 0 us,   Bresenham = 0 us
Match: NO    (plotsum=165367963863)

[TAD 8.8 Fixed-Point]
................................................
..##############################################
................................................

[Bresenham Algorithm]
................................................
..##############################################
................................................

Batch x10000:
  TAD 8.8 total = 641 us  (avg 64.1 ns/draw)
  Bresenham total = 799 us  (avg 79.9 ns/draw)
```

## Algorithm Details

### TAD 8.8 Fixed-Point
- Uses 8.8 fixed-point arithmetic (8 bits integer, 8 bits fractional)
- Branchless design for consistent performance
- Inspired by Terry A. Davis's TempleOS graphics routines
- Generally 20-30% faster than Bresenham

### Bresenham Algorithm
- Classic integer-only line drawing
- Developed by Jack Bresenham in 1965
- Uses error accumulation for pixel selection
- Industry standard for decades

## Requirements

- C++17 compatible compiler
- Standard library support for `<chrono>`, `<vector>`, `<iostream>`, etc.

## License

This code is provided as educational material and tribute to Terry A. Davis's contributions to computer graphics and systems programming.
