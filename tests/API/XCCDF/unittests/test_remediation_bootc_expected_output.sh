#!/bin/bash
dnf -y install \
    rsyslog \
    reboot \
    podman

dnf -y remove \
    usbguard
