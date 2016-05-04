#define HELLO_MAJOR 250

#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1)
#define LEDCMD_GET_STATE _IOR(HELLO_MAJOR, 2, unsigned char *)
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *)
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *)

/* define name of the drivers */
#define FILENAME "/dev/hello"

/* initial state of LEDs */
#define INITIAL_STATE 0x00

#define BP "%d%d%d%d%d%d%d%d"
#define BB(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)


typedef struct led {
    int pin;
    unsigned char value;
} led_t;


