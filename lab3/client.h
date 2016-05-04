#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "hello.h"

#define STATE_ALL -1

void print_usage(void);
void led_reset(void);
void led_state(int led_n);
void led_off(int led_n);
void led_on(int led_n);


void led_reset(void) {
    int fd;
    /* open device /dev/hello */
    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (ioctl(fd, LEDCMD_RESET_STATE) == -1) {
        fprintf(stderr, "RESET ERROR!\n");
        return;
    }
    printf("RESET LEDs\n");
    close(fd);
}

void led_state(int led_n) {
    int fd;
    led_t led;
    /* state of all LEDs at once in one byte */
    char state;

    /* open device /dev/hello */
    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (led_n == STATE_ALL) {
        if (ioctl(fd, LEDCMD_GET_STATE, &state) == -1) {
            fprintf(stderr, "STATE ERROR\n");
            return;
        }
        for (led_n = 0 ; led_n < 8; led_n++) {
            printf("LED %d is %s\n", led_n,
                   (state & (1 << led_n)) ?
                   "on" : "off");
        }
    } else {
        led.pin = led_n;
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED STATE ERROR\n");
            return;
        }
        printf("LED %d is %s\n", led.pin,
               led.value ? "on" : "off");
    }

    close(fd);
}


void led_off(int led_n) {
    int fd;
    led_t led;

    /* open device /dev/hello */
    if ((fd = open(FILENAME, O_RDWR)) == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* disable pin */
    led.value = 0;
    if (ioctl(fd, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TRURN OFF ERROR\n");
        return;
    } else {
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED TURN OFF ERROR\n");
            return;
        }
        printf("LED %d is %s\n", led.pin, led.value ?
                "on" : "off");
    }
    close(fd);
}

void led_on(int led_n) {
    int fd;
    led_t led;

    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* enable pin */
    led.value = 1;
    if (ioctl(fd, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TURN ON ERROR\n");
        return;
    } else {
        if (ioctl(fd, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "TURN ON ERROR!\n");
            return;
        }
        printf("LED %d is %s\n", led.pin, led.value ?
                "on" : "off");
    }
    close(fd);
}
