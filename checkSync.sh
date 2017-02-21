#!/bin/bash

echo '?' > /dev/ttyUSB0
date +%s
head -n1 /dev/ttyUSB0
date +%s
