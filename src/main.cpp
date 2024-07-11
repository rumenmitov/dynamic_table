#include <iostream>

#include "dyn_table.h"

using namespace std;
int main(void) {

  try {
    DynTable dyn_table;

    uint64_t offset = dyn_table.get_chunk(5);
    char* message = (char*) (dyn_table.base + offset);

    message[0] = 'H';
    message[1] = 'e';
    message[2] = 'l';
    message[3] = 'l';
    message[4] = 'o';

    cout << "Message was: " << message << endl;

    dyn_table.return_chunk(offset);
  } catch (DynTableError err) {
    cerr << "Error encountered!" << endl;
  }
  
  return 0;
}
