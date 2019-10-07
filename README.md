# d1-mini-sht30-prometheus
Code to make a Wemos/Lolin D1 Mini read and push temperature and humidity from a SHT30 sensor to a Prometheus Pushgateway, with optional output on an OLED display.

One of the problems with the D1 Mini is that if you're using the official SHT30 sensor hat/shield, the proximity of the sensor to the D1 Mini will result in excess heat throwing off both temperature and humidity readings.

You can either:
- Break out the SHT30 and put it on a cable some distance away from the D1 Mini;
- Or use the deep sleep functionality to enter a low power state which emits much less heat, making your readings closer to actual values.

I opted for the second as a smaller, more discreet sensor was important.

(Ab)uses ifdefs to enable/disable functionality.
