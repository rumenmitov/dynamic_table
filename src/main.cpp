#include <cstdint>
#include <cstdio>
#include <iostream>

#include "dyn_table.h"

using namespace std;
int main(void) {

  uint64_t bytes;

  cout << "Enter the number of ASCII characters that you need for your message: ";
  cin >> bytes;
  bytes += 1; /* NOTE This is for the null-terminator. */

  getchar();

  cout << "Enter message: ";

  try {
    DynTable dyn_table;

    uint64_t offset = dyn_table.get_chunk(bytes);
    char* message = (char*) (dyn_table.base + offset);

    cin.getline(message, bytes);
    message[bytes] = '\0';

    cout << "Message was: " << message << endl;

    dyn_table.return_chunk(offset);
  } catch (DynTableError err) {
    cerr << "Error encountered!" << endl;
  }
  
  return 0;
}
