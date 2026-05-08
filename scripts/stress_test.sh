#!/bin/bash

for i in {1..20}
do
    ./scripts/triage.sh Patient$i $((20+i)) $((RANDOM%10+1)) &
    sleep 0.2
done
