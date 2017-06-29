#include <drivers/gpio.h>
#include <unistd.h>
#include <kernel/printk.h>

#include <embox/unit.h>

static int start_dht11(void){

    struct gpio* dht = gpio_by_num(3);
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);

    unsigned int counter = 0;
    while(1){
        printk("Counter: %d\n", counter);

        if(counter%2){
            gpio_set_level(dht, 0, 1);
        } else {
            gpio_set_level(dht, 0, 0);
        }

        counter++;
        sleep(1);
    }
}

EMBOX_UNIT_INIT(start_dht11);
