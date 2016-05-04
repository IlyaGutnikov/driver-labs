#include "client.h"

int main(int argc, char **argv) {
    char *reset = "reset";
    char *ledst= "ledstate";
    char *on = "on";
    char *off = "off";

    switch (argc) {

    case 2:
        if  (strcmp (argv[1], reset) == 0) {
            led_reset();
        } else if (strcmp (argv[1], ledst) == 0) {
            led_state(STATE_ALL);
        } else {
            print_usage();
        }
        break;

    case 3:
        if  (strcmp (argv[1], ledst) == 0) {
            led_state(atoi(argv[2]));
        } else if (strcmp (argv[1], off) == 0) {
            led_off(atoi(argv[2]));
        } else if (strcmp (argv[1], on) == 0) {
            led_on(atoi(argv[2]));
        }
        break;
    default:
        print_usage();
    }
    return 0;
}

void print_usage(void) {
    puts("Illegal syntax. The usage is: \n"
         "client { reset | ledstate | on | off } [N]\n"
         "\t where { ledstate | on | off } could be followed\n"
         "\t by optional number [N] of particular led");
}

