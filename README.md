# piuio2rp2040

Ever wanted to get the status of a PIUIO on a microcontroller? Well now you can!

# What is this?

It's an RP2040 firmware that will host a usb PIUIO and translate that into a usb HID gamepad and take lights from an HID endpoint.

...kind of. It's a skeleton and a basis. Feel free to finish it and make a PR.

I used an [Adafruit RP2040 feather host](https://www.adafruit.com/product/5723) dev board for development.

# Why?

Because I have lost control of my life.

# Why is it incomplete?

Performance over usb control transfer requests using the rp2040's PIO host was very subpar in my very limited testing. 

For one round trip, not even accounting for the four sensor channels of the proper piuio I was only getting around 142Hz polling out of the device. If I turned off the push control transfer, I was able to get it to around double for 250Hz.

But overall this was just a fun experiment I thought some others would enjoy seeing. RP2040's USB PIO host cores are quite cool.

# Credits

- [rp2040-gamecon](https://github.com/Drewol/rp2040-gamecon/) for the basic USB HID Gamepad firmware.
- [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) for the USB host PIO cores. Very cool project!