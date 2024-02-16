#!/bin/bash

ECHO_COLOR="\033[1;32m"

echo -e "${ECHO_COLOR}[/deps] building limine\033[0m"
make -C limine
