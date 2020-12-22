#include <iostream>
#include "yasi.hpp"
#include "main.h"

using namespace std;

int main(int argc, char* argv[]) {

    std::yasi s = "Hello World";

    cout << "string size: " << s.size() << " chars" << endl;
    cout << "Type: " << internal_type_str(s) << endl;

    for(int i = 0; i < 20; i++) {
        s.append('r');
    }

    cout << "string size: " << s.size() << " chars" << endl;
    cout << "Type: " << internal_type_str(s) << endl;

    s.append('q');

    cout << "string size: " << s.size() << " chars" << endl;
    cout << "Type: " << internal_type_str(s) << endl;

    return 0;
}
