#!/usr/bin/env python2

import socket
import argparse
import subprocess
import os, re, threading
import time
from timeit import default_timer as timer

from struct import pack


def encrypt(string):
	key = 171
	result = pack('>I', len(string))
	for i in string:
		a = key ^ ord(i)
		key = a
		result += chr(a)
	return result

def decrypt(string):
	key = 171
	result = ""
	for i in string:
		a = key ^ ord(i)
		key = ord(i)
		result += chr(a)
	return result


def mean(a):
    sum = 0
    for i in a:
        sum += i
    return sum / len(a)


class energy_tool():
    def __init__(self, ip, port):
        self.sock_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock_tcp.connect((ip, port))
        self.energy_command = '{"emeter":{"get_realtime":{}}}'
        self.regexp_power = re.compile('.*"power_mw":(\d*)')
        self.current_energy = 0.0
        self.ref_power = 0.0
        self.is_calibrating = False
        self.powers = []

    def get_power(self):
        self.sock_tcp.send(encrypt(self.energy_command))
        data = self.sock_tcp.recv(2048)
        emeter_read = decrypt(data[4:])
        power = float(self.regexp_power.match(emeter_read).groups()[0]) * 0.001
        return power

    def get_energy(self, power):
        tick_time = timer()
        delta_t = tick_time - self.tick
        energy = power * delta_t
        self.tick = tick_time
        return energy, delta_t

    def calibrate(self):
        self.powers.append(self.get_power())

    def measure(self):
        power = self.get_power()
        delta_power = power - self.ref_power
        if delta_power > 0:
            e, delta_t = self.get_energy(delta_power)
            self.current_energy += e

    def trigger(self):
        if self.is_calibrating:
            self.calibrate()
        else:
            self.measure()


class timer_tool(threading.Thread):
    def __init__(self, energy_tool):
        threading.Thread.__init__(self)

        self.energy_tool = energy_tool
        self.stop_event = threading.Event()

    def stop(self):
        self.stop_event.set()

    def stopped(self):
        return self.stop_event.is_set()

    def run(self):
        self.energy_tool.tick = timer()
        while not self.stopped():
            self.energy_tool.trigger()


parser = argparse.ArgumentParser(description="Software Energy measurements with TP-Link Wi-Fi Smart Plug Client")
parser.add_argument("-t", "--target", metavar="<hostname>", required=True, help="Target hostname or IP address")
parser.add_argument("-c", "--command", metavar="<command>", required=True, help="Command to launch program")
parser.add_argument("-m", "--number_measures", metavar="<number_measures>", help="Number of measures")
args = parser.parse_args()

FNULL = open(os.devnull, 'w')

et = energy_tool(args.target, 9999)

# Calibration phase

#print "Start calibration phase"
# calibration_timer = timer_tool(et)
# et.is_calibrating = True
# calibration_timer.start()
# res = subprocess.Popen(["sleep", "5"], stdout=FNULL, stderr=subprocess.STDOUT)
# res.wait()
# calibration_timer.stop()
# calibration_timer.join()
# meaned_powers = mean(et.powers)
#print "mean power measured ", meaned_powers

et.ref_power = 0

# Measure phase
et.is_calibrating = False

energy_measures = []
time_elapsed    = []

if args.number_measures is not None:
    number_measures = int(args.number_measures)
    #print "execute ", number_measures, " times"
else:
    number_measures = 10
    #print "default number_measures = ", number_measures

core = 0
energy_threshold = 10.0

for i in xrange(number_measures):
    time.sleep(1)
    counter = 0
    et.current_energy = 0
    measure_timer = timer_tool(et)
    measure_timer.start()
    program_starts = timer()
    while et.current_energy <  energy_threshold:
        res = subprocess.Popen(["taskset", "-c", str(core)] + args.command.split(), stdout=FNULL, stderr=subprocess.STDOUT)
        res.wait()
        core = core + 1
        if core > 7:
            core = 0
        counter = counter + 1

    program_ends = timer()
    measure_timer.stop()
    measure_timer.join()


    energy_measures.append(et.current_energy / counter)
    time_elapsed.append((program_ends - program_starts) / counter)

FNULL.close()
et.sock_tcp.close()

print str(mean(energy_measures)) + "," + str(mean(time_elapsed) * 1000)
