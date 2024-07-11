#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <list>
#include <sys/mman.h>

/* Virtual address */
typedef size_t addr_t;

/*
 * @brief Possible throw errors of the DynTable methods.
 *
 * FailedInit - initialization failed
 * FailedExt  - could not extend memory pool
 */
enum DynTableError { FailedInit, FailedExt };

/*
 * @brief Class for that manages virtual memory with the help of a dynamic
 * table.
 */
class DynTable {

public:
  static const uint16_t PAGE_SIZE = 4096;
  static const uint16_t DEFAULT_PAGE_COUNT = 1000;

  /* NOTE base represesnts the start of the memory pool.
   * This is needed for when the pool needs to be extended as the base location
   * could change.
   */
  addr_t* base = (addr_t*) 0x00;

  /*
   * @brief Constructor of DynTable with user-specified amount of memory.
   * @param uint64_t bytes = DEFAULT_PAGE_COUNT * PAGE_SIZE The initial size of the memory pool.
   * @throws DynTableError
   */
  DynTable(uint64_t bytes = DEFAULT_PAGE_COUNT * PAGE_SIZE) : _n_pages((bytes / PAGE_SIZE) + 1) {
    void *pool_init =
        mmap(nullptr, _n_pages * PAGE_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANON, -1, PAGE_SIZE);

    if (pool_init == MAP_FAILED)
      throw FailedInit;

    base = (addr_t*) pool_init;

    _pool.push_back(new Block{.offset = 0, .size = _n_pages, .state = Free});
  }

  /*
   * @brief Destructor of DynTable.
   */
  ~DynTable() {
    if (_pool.empty())
      return;

    assert(_pool.front() != nullptr && "Memory was corrupted!");

    munmap((void *)base, _n_pages * PAGE_SIZE);

    while (!_pool.empty()) {
      delete _pool.back();
      _pool.pop_back();
    }
  }

  /*
   * @brief Interface to borrow virtual memory.
   * @param uint64_t bytes The amount of memory needed.
   * @returns uint64_t An offset of the base address, signifying the start of
   * the chunk.
   * @throws DynTableError
   */
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

	
        _pool.insert(block_it, used_block);
        _pool.insert(std::next(block_it, 1), free_block);
	_pool.remove(*block_it);

        return used_block->offset;
      }
    }

    extend(n_pages);
    return _pool.back()->offset;
  }

  /*
   * @brief Interface to return borrowed virtual memory.
   * @param uint64_t offset The offset from base, signifying the start of the
   * chunk.
   */
  void return_chunk(uint64_t offset) noexcept {
    for (auto block_it = _pool.begin(); block_it != _pool.end(); block_it++) {
      if ((*block_it)->offset != offset)
        continue;

      (*block_it)->state = Free;

      auto pred = std::prev(block_it, 2);
      auto succ = std::next(block_it, 1);

      if (succ != _pool.end() && (*succ)->state == Free) {
        (*block_it)->size += (*succ)->size;
        _pool.remove(*succ);
      }

      if (block_it != _pool.begin() && (*pred)->state == Free) {
        (*pred)->size += (*block_it)->size;
        _pool.remove(*block_it);
      }

      return;
    }
  }

private:
  enum BlockState { Free, Used };

  struct Block {
    uint64_t offset = 0x00;
    unsigned long long size = 0;
    BlockState state;
  };

  /* NOTE Contains all the partition blocks. */
  std::list<Block *> _pool;

  /* NOTE Keeps track of the total number of pages reserved.*/
  uint64_t _n_pages = DEFAULT_PAGE_COUNT;

  /*
   * @brief Extends the memory pool. Could potentially change the base address.
   * @param uint64_t extra_pages The amount of pages needed.
   * @throws DynTableError
   */
  void extend(uint64_t extra_pages) {
    void *pool_ext =
        mremap((void *)base, _n_pages * PAGE_SIZE,
               (_n_pages + extra_pages) * PAGE_SIZE, MREMAP_MAYMOVE);

    if (pool_ext == MAP_FAILED)
      throw FailedExt;

    base = (addr_t*) pool_ext;
    _n_pages += extra_pages;

    Block *last_block = _pool.back();

    if (last_block->state == Free)
      last_block->size += extra_pages;
    else {
      _pool.push_back(
          new Block{.offset = last_block->offset + last_block->size * PAGE_SIZE,
                    .size = extra_pages,
                    .state = Free});
    }
  }
};
