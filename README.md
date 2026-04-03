# char_driver

Simple Linux character driver example implemented as an out-of-tree kernel module.

## Project structure

- `driver/char_driver.c`: character driver source code
- `driver/Makefile`: kernel module build rules
- `Makefile`: top-level build entry point for the driver and userspace app
- `app/ioctl_app.c`: userspace example for the custom `ioctl`
- `app/Makefile`: build rules for the userspace `ioctl` example
- `driver/char_driver.ko`: compiled kernel module generated after `make`

## Build

Compile the driver and userspace app with:

```bash
make
```

Clean generated build files with:

```bash
make clean
```

## Load and test

1. Insert the module:

```bash
sudo insmod driver/char_driver.ko
```

You can also set the initial message when loading the module:

```bash
sudo insmod driver/char_driver.ko message="hello from sysfs"
```

2. Check the kernel log and note the assigned major number:

```bash
lsmod | grep char_driver
dmesg | tail
```

3. Create the device file using the major number reported by the driver:

```bash
sudo mknod /dev/char_dev c <major_number> 0
sudo chmod 666 /dev/char_dev
```

4. Write to and read from the device:

```bash
echo "hello kernel" > /dev/char_dev
cat /dev/char_dev
```

5. Clear the stored message with the custom `ioctl`:

```bash
make -C app
./app/ioctl_app
```

After calling the `ioctl`, reading from `/dev/char_dev` returns EOF until a new message is written through the device or sysfs.

6. Read or update the `message` sysfs parameter:

```bash
cat /sys/module/char_driver/parameters/message
echo "updated from sysfs" | sudo tee /sys/module/char_driver/parameters/message
cat /dev/char_dev
```

7. Remove the module when finished:

```bash
sudo rmmod char_driver
```

## Notes

- The device name registered by the driver is `char_dev`.
- The module requests a dynamic major number at load time, so the value can change between runs.
- The driver stores up to 255 bytes from the last write and returns that message on read.
- The custom `ioctl` command `CHAR_DRIVER_IOCTL_CLEAR_MESSAGE` clears the stored message.
- The module parameter `message` is exposed through `/sys/module/char_driver/parameters/message`.
