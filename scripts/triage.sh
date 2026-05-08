#!/bin/bash

if [ $# -ne 3 ]
then
    echo "Usage: ./triage.sh <name> <age> <severity>"
    exit 1
fi

NAME=$1
AGE=$2
SEVERITY=$3

if ! [[ "$AGE" =~ ^[0-9]+$ ]]
then
    echo "Invalid age"
    exit 1
fi

if ! [[ "$SEVERITY" =~ ^[0-9]+$ ]]
then
    echo "Invalid severity"
    exit 1
fi

if [ $SEVERITY -ge 9 ]
then
    PRIORITY=1
    CARE=3
elif [ $SEVERITY -ge 7 ]
then
    PRIORITY=2
    CARE=3
elif [ $SEVERITY -ge 5 ]
then
    PRIORITY=3
    CARE=2
else
    PRIORITY=4
    CARE=1
fi

python3 - << EOF
import os
import struct
import time

fifo = os.open('/tmp/triage_fifo', os.O_WRONLY)

name = "$NAME".encode().ljust(64, b'\0')

record = struct.pack(
    'i64siiiiq',
    int(time.time()) % 10000,
    name,
    $AGE,
    $SEVERITY,
    $PRIORITY,
    $CARE,
    int(time.time())
)

os.write(fifo, record)
os.close(fifo)
EOF
