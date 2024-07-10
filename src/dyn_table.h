#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <list>
#include <iterator>
#include <sys/mman.h>

typedef size_t addr_t;

enum DynTableError { FailedInit, PoolFull, FailedExt };

class DynTable {

public:
  static const uint16_t PAGE_SIZE = 4096;
  static const uint16_t DEFAULT_PAGE_COUNT = 1000;

  addr_t base = 0x00;

  DynTable(uint64_t bytes = DEFAULT_PAGE_COUNT * PAGE_SIZE) : n_pages(bytes / PAGE_SIZE) {
    void *pool_init =
      mmap(nullptr, n_pages * PAGE_SIZE, PROT_READ | PROT_WRITE,
	   MAP_PRIVATE | MAP_ANON, 0, PAGE_SIZE);

    if (pool_init == MAP_FAILED)
      throw FailedInit;

    base = (addr_t)pool_init;

    _pool.push_back(new Block{.offset = 0, .size = n_pages, .state = Free});

    _pool_partitions = 1;
  }

  ~DynTable() {
    if (_pool.empty())
      return;

    assert(_pool.front() != nullptr && "Memory was corrupted!");

    munmap((void *)base, n_pages * PAGE_SIZE);

    while (!_pool.empty()) {
      delete _pool.back();
      _pool.pop_back();
    }
  }

  uint64_t get_chunk(uint64_t bytes) {
    uint64_t n_pages = (bytes / PAGE_SIZE) + 1;

    for (auto block_it = _pool.begin(); block_it != _pool.end(); block_it++) {
      if ((*block_it)->size >= n_pages && (*block_it)->state == Free) {
        Block *used_block = new Block{
	  .offset = (*block_it)->offset, .size = n_pages, .state = Used};

        Block *free_block = new Block{
	  .offset = used_block->offset + used_block->size * PAGE_SIZE,
	  .size = (*block_it)->size - used_block->size,
	  .state = Free};

        _pool.remove(*block_it);
        _pool.insert(block_it, used_block);
        _pool.insert(block_it++, used_block);

        return used_block->offset;
      }
    }

    extend(n_pages);
    return _pool.back()->offset;
  }

  void return_chunk(uint64_t offset) {
    for (auto block_it = _pool.begin(); block_it != _pool.end(); block_it++) {
      if ((*block_it)->offset != offset) continue;

      (*block_it)->state = Free;

      auto pred = std::prev(block_it, 2);
      auto succ = std::next(block_it, 1);

      if ((*succ)->state == Free) {
	(*block_it)->size += (*succ)->size;
	_pool.remove(*succ);
	_pool_partitions--;
      }

      if ((*pred)->state == Free) {
	(*pred)->size += (*block_it)->size;
	_pool.remove(*block_it);
	_pool_partitions--;
      }
    }
  }

private:
  enum BlockState { Free, Used };

  struct Block {
    uint64_t offset = 0x00;
    unsigned long long size = 0;
    BlockState state;
  };

  std::list<Block *> _pool;
  uint64_t n_pages = 0;
  uint64_t _pool_partitions = 0;

  void extend(uint64_t extra_pages) {
    void *pool_ext = mremap((void *)base, n_pages * PAGE_SIZE,
                            (n_pages + extra_pages) * PAGE_SIZE, MREMAP_MAYMOVE);

    if (pool_ext == MAP_FAILED)
      throw FailedExt;

    base = (addr_t)pool_ext;

    Block *last_partition = _pool.back();

    if (last_partition->state == Free)
      last_partition->size += extra_pages;
    else {
      _pool.push_back(new Block{.offset = last_partition->offset +
                                          last_partition->size * PAGE_SIZE,
                                .size = extra_pages,
                                .state = Free});
    }
  }
};
