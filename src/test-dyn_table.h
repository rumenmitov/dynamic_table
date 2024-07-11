#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include "dyn_table.h"

namespace Tests {
  inline void memory_allocation_and_release(void) {
    std::cout << "Testing memory_allocation_and_release...";

    DynTable dyn_table;
    uint64_t offset = dyn_table.get_chunk(5);

    int *arr = (int*) (offset + dyn_table.base);

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;

    assert(arr[0] == 1);
    assert(arr[1] == 2);
    assert(arr[2] == 3);
    assert(arr[3] == 4);
    assert(arr[4] == 5);

    dyn_table.return_chunk(offset);

    std::cout << "passed.\n";
  }

  inline void extending_memory(void) {
    std::cout << "Testing extending_memory...";

    try {
      DynTable dyn_table(1);
      uint64_t offset = dyn_table.get_chunk(100 * DynTable::PAGE_SIZE);
      dyn_table.return_chunk(offset);
    } catch (DynTableError e) {
      std::cerr << "Failed to extend memory!\n";
      exit(1);
    }

    std::cout << "passed.\n";
  }
}

namespace Performance {
  using namespace std::chrono;
  
  inline uint64_t extending_memory(uint64_t bytes) {
    DynTable dyn_table(1);
    
    auto start = high_resolution_clock::now();
    uint64_t offset = dyn_table.get_chunk(bytes);
    auto end = high_resolution_clock::now();

    dyn_table.return_chunk(offset);

    return duration_cast<nanoseconds>(end - start).count();
  }
}
