obj-$(CONFIG_MTK_SMART_BATTERY) += battery_meter.o battery_common.o
obj-$(CONFIG_THUNDERCHARGE_CONTROL) += thundercharge_control.o

ifeq ($(MTK_FAN5405_SUPPORT),yes)
    obj-$(CONFIG_MTK_SMART_BATTERY) += switch_charging.o 
else
    obj-$(CONFIG_MTK_SMART_BATTERY) += linear_charging.o
endif
