#include <cstdint>
#include <cstdio>
#include <iostream>

#include "dyn_table.h"

#ifdef DEBUG
#include "test-dyn_table.h"
#endif

using namespace std;
int main(void) {

#ifdef DEBUG
  Tests::memory_allocation_and_release();
  Tests::extending_memory();
#endif

  cout << endl;
  cout << "Dynamic Table Memory Allocation Program\n";
  cout << "---------------------------------------\n";

  uint64_t bytes;

  cout << "Enter the number of ASCII characters that you need for your "
          "message: ";
  cin >> bytes;
  bytes += 1; /* NOTE This is for the null-terminator. */

  getchar();

  cout << "Enter message: ";

  try {
    DynTable dyn_table;

    uint64_t offset = dyn_table.get_chunk(bytes);
    char *message = (char *)(dyn_table.base + offset);

    cin.getline(message, bytes);
    message[bytes] = '\0';

    cout << "Message was: " << message << endl;

    dyn_table.return_chunk(offset);
  } catch (DynTableError err) {
    cerr << "Error encountered!" << endl;
  }

  return 0;
}
