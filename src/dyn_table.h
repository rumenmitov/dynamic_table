#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <list>
#include <sys/mman.h>

typedef size_t addr_t;

enum DynTableError { FailedInit, PoolFull };

class DynTable {

public:
  static const uint16_t PAGE_SIZE = 4096;
  static const uint16_t DEFAULT_PAGE_COUNT = 1000;

  DynTable() {
    void *pool_init =
        mmap(nullptr, _pool_size * PAGE_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANON, 0, PAGE_SIZE);

    if (pool_init == MAP_FAILED)
      throw FailedInit;

    _pool.push_back(new Block{
        .addr = (addr_t)pool_init, .size = _pool_size, .state = Free});

    _pool_size = DEFAULT_PAGE_COUNT;
    _pool_partitions = 1;
  }

  ~DynTable() {
    if (_pool.empty())
      return;

    assert(_pool.front() != nullptr && "Memory was corrupted!");

    munmap((void *)_pool.front()->addr, _pool_size * PAGE_SIZE);

    while (!_pool.empty()) {
      delete _pool.back();
      _pool.pop_back();
    }
  }

  addr_t get_chunk(uint64_t bytes) {
    uint64_t n_pages = (bytes / PAGE_SIZE) + 1;

    for (auto block_it = _pool.begin(); block_it != _pool.end(); block_it++) {
      if ((*block_it)->size >= n_pages && (*block_it)->state == Free) {
        Block *used_block = new Block{
            .addr = (*block_it)->addr, .size = n_pages, .state = Used};

        Block *free_block =
            new Block{.addr = used_block->addr + used_block->size * PAGE_SIZE,
                      .size = (*block_it)->size - used_block->size,
                      .state = Free};

	_pool.remove(*block_it);
	_pool.insert(block_it, used_block);
	_pool.insert(block_it++, used_block);

	return used_block->addr;
      }

      // TODO implement extension of dynamic pool
    }
  }

private:
  enum BlockState { Free, Used };

  struct Block {
    addr_t addr = 0x00;
    unsigned long long size = 0;
    BlockState state;
  };

  std::list<Block *> _pool;
  uint64_t _pool_size = 0;
  uint64_t _pool_partitions = 0;
};
