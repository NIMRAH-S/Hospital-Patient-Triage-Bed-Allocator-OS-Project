#!/bin/bash

PID=$(cat hospital.pid)

kill -SIGTERM $PID

ipcrm -M 0xBEDF00D

rm -f /tmp/discharge_fifo
rm -f /tmp/triage_fifo

rm -f hospital.pid

echo "Hospital stopped successfully"
