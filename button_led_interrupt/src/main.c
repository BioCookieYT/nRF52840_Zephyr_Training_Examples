
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


#define LED0_NODE DT_ALIAS(led0)
#define BTN0_NODE DT_ALIAS(sw0)


static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);

void btn_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led);
}

void main(void)
{
        int ret;
    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    ret += gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        return;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        return;
    }
    
    static struct gpio_callback button_cb_data;
    gpio_init_callback(&button_cb_data, btn_callback, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);


    while (1) {
        k_sleep(K_MSEC(1000));
    }
}


