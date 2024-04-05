## Requirements

- nRF Connect SDK
- Linux PC or Raspberry Pi (tested with Ubuntu 23.10)
- Bluetooth LE dongle or embedded controller
- nRF5340 DK - For the radio Co-Processor (RCP) device
- nRF DK compatible with matter accessory samples (e.g. nRF5340 or nRF52840)

## RCP Setup

[Original instructions](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/protocols/thread/tools.html#configuring_a_radio_co-processor)

To use nRF5340 DK as an RCP first we need to create new nRF application
project using the RCP sample from nrf/samples/opentreaad/coprocessor.

Once the project is created add the following config option to `prj.conf`.

```conf
CONFIG_IEEE802154=y
```

Then we need two new devicetree overlays `nrf5340dk_nrf5340_cpuapp.overlay` and
`nrf5340dk_nrf5340_cpunet`.

Create the cpuapp overlay to `boards/nrf5340dk_nrf5340_cpuapp.overlay` and add
the following content to it.

```dts
/ {
	chosen {
		zephyr,console = &uart0;
		zephyr,ot-uart = &uart0;
		zephyr,shell-uart = &uart0;
		zephyr,uart-mcumgr = &uart0;
		zephyr,bt-mon-uart = &uart0;
		zephyr,bt-c2h-uart = &uart0;
		zephyr,bt-hci-rpmsg-ipc = &ipc0;
		nordic,802154-spinel-ipc = &ipc0;
		zephyr,ieee802154 = &ieee802154;
	};
};

&ieee802154 {
    status = "okay";
};
```

Then create the cpunet overlay to
`child_image/802154_rpmsg/boards/nrf5340dk_nrf5340_cpunet.overlay` and add
the content below to it.

```dts
/ {
	model = "Nordic nRF5340 DK NRF5340 Network";
	    compatible = "nordic,nrf5340-nrf5340-cpunet";

	chosen {
		zephyr,console = &uart0;
		zephyr,shell-uart = &uart0;
		zephyr,uart-mcumgr = &uart0;
		zephyr,bt-mon-uart = &uart0;
		zephyr,bt-c2h-uart = &uart0;
		zephyr,bt-hci-rpmsg-ipc = &ipc0;
		nordic,802154-spinel-ipc = &ipc0;
		zephyr,sram = &sram1;
		zephyr,flash = &flash1;
		zephyr,code-partition = &slot0_partition;
		zephyr,ieee802154 = &ieee802154;
	};
};

&ieee802154 {
	status = "okay";
};
```

After this we can flash the device with the RCP project.

```sh
west build -p -b nrf5340dk_nrf5340_cpuapp
west flash
```

## Setting up OTBR (OpenThread Border Router)

[Original instructions](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/protocols/thread/tools.html#running_otbr_using_docker)

You can follow the original instructions, but remember to change baudrate and
and device to the application core's serial port in the docker start
command. The default baud on nRF5340 is `115200` and application core is
commonly found on `/dev/ttyACM1`.

```sh
sudo docker run -it --rm --privileged --name otbr --network otbr -p 8080:80 \
--sysctl "net.ipv6.conf.all.disable_ipv6=0 net.ipv4.conf.all.forwarding=1 net.ipv6.conf.all.forwarding=1" \
--volume /dev/ttyACM1:/dev/radio nrfconnect/otbr:98dda6c \
--radio-url spinel+hdlc+uart:///dev/radio?uart-baudrate=115200
```
