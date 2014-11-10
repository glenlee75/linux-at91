adb shell rm /system/etc/firmware/wifi_firmware.bin
adb shell rm /system/etc/firmware/wifi_firmware_ap.bin
adb shell rm /system/etc/firmware/wifi_firmware_p2p_concurrency.bin
adb push ../../core/wifi_v111/ASIC_A0/wifi_firmware.bin /system/etc/firmware/wifi_firmware.bin
adb push ../../core/wifi_v111/ASIC_A0_AP/wifi_firmware_ap.bin /system/etc/firmware/wifi_firmware_ap.bin
adb push ../../core_concurrency/wifi_v111/ASIC_A0/wifi_firmware_p2p_concurrency.bin /system/etc/firmware/wifi_firmware_p2p_concurrency.bin
adb shell rm /system/vendor/modules/nmi_wifi.ko
adb push binary/linux/ARM-3.4/nmi_wifi.ko  /system/vendor/modules/nmi_wifi.ko
