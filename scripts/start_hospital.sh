#!/bin/bash

echo "================================="
echo " Hospital Management System"
echo "================================="

mkdir -p logs

gcc -Wall -pthread src/admissions.c src/scheduler.c -o admissions

gcc -Wall -pthread src/patient_simulator.c -o patient_simulator

./admissions &

ADM_PID=$!

echo $ADM_PID > hospital.pid

echo "Hospital started with PID $ADM_PID"
