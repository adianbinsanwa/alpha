#include "../header/decoder.h"

char *
d_boolean(int res) {
    switch (res) {
        case 0:
             return "False";
        case 1:
             return "True";
        default:
             return "invalid bool";
    }
};


