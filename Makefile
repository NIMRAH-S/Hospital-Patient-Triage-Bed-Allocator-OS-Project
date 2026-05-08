all:
	gcc -Wall -pthread src/admissions.c src/scheduler.c -o admissions
	gcc -Wall -pthread src/patient_simulator.c -o patient_simulator

run:
	bash scripts/start_hospital.sh

clean:
	rm -f admissions
	rm -f patient_simulator
	rm -f *.o
	rm -f logs/*.txt
	rm -f hospital.pid

test:
	bash scripts/stress_test.sh
