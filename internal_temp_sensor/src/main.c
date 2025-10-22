
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>

/* nRF52840 internal die temperature sensor */
#define TEMP_NODE DT_NODELABEL(temp)

void main(void)
{
	const struct device *temp_dev = DEVICE_DT_GET(TEMP_NODE);
	
	if (!device_is_ready(temp_dev)) {
		printk("Error: nRF52840 temperature sensor not ready\n");
		return;
	}

	printk("nRF52840 die temperature sensor initialized\n");

	while (1) {
		struct sensor_value temp;
		int ret;

		/* Fetch sensor sample */
		ret = sensor_sample_fetch(temp_dev);
		if (ret) {
			printk("Error: Failed to fetch sensor sample (err %d)\n", ret);
			k_sleep(K_SECONDS(1));
			continue;
		}

		/* Get temperature value */
		ret = sensor_channel_get(temp_dev, SENSOR_CHAN_DIE_TEMP, &temp);
		if (ret) {
			printk("Error: Failed to get temperature reading (err %d)\n", ret);
			k_sleep(K_SECONDS(1));
			continue;
		}

		/* Print temperature (val1 = integer, val2 = fractional in micro-units) */
		printk("Die temperature: %d.%06d C\n", temp.val1, temp.val2);

		/* Wait 2 seconds before next reading */
		k_sleep(K_SECONDS(2));
	}
}


