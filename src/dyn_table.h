#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/mman.h>


typedef size_t addr_t;

enum DynTableError {
    FailedInit,
};

class DynTable {


    public:


        static const uint16_t PAGE_SIZE = 4096;
        static const uint16_t DEFAULT_PAGE_COUNT = 1000;


        DynTable() {
            _pool = new Block*[DEFAULT_PAGE_COUNT];

            void *pool_init = mmap(
                    nullptr,
                    DEFAULT_PAGE_COUNT * PAGE_SIZE,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANON,
                    0,
                    0
                    );

            if (pool_init == MAP_FAILED) throw FailedInit;

            for (uint64_t i = 0; i < DEFAULT_PAGE_COUNT; i++) {
                register_block((addr_t) pool_init + i * PAGE_SIZE);
            }

        }

        ~DynTable() {
            if (!_pool) return;

            munmap(
                    (void*) _pool[0]->addr,
                    _pool_size * PAGE_SIZE
                  );

            for (uint64_t i = 0; i < _pool_size; i++) {
                delete _pool[i];
            }

            delete[] _pool;
        }


    private:


        enum BlockState {
            Free,
            Used
        };

        struct Block {
            addr_t addr;
            BlockState state;
            Block *next = nullptr;
        };

        Block** _pool = nullptr;
        uint64_t _pool_size = 0;

        void register_block(addr_t addr) {
            Block* new_block = new Block { 
                .addr = addr,
                .state = Free,
                .next = nullptr 
            };

            _pool[_pool_size++] = new_block;
        }
};
