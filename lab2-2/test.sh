#!/bin/sh
sudo insmod ./pid.ko
sudo dmesg -c
cat /proc/pid
echo "1" > /proc/pid
sudo dmesg -c
sudo cat /proc/pid
echo "132413213" > /proc/pid
sudo dmesg -c
cat /proc/pid
sudo rmmod pid
sudo dmesg -c
