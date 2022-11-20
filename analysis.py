#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
import numpy as np
from enum import Enum


class CurrentProblem(Enum):
    PHILOSOPHERS = "philosophers"
    READER_WRITER = "reader_writer"
    PRODUCER_CONSUMER = "producer_consumer"


def main():
    with open("philosophers.csv") as file:
        reader = csv.reader(file, delimiter=",")

        threads = []
        y = []
        
        y_err = []
        for row in reader:
            threads.append(row[0])
            float_row = []
            # float_row = [float(i) for i in row]
            for i in row[1:]:
                time_list = i.split(";")
                duration = float(time_list[0]) - float(time_list[1])
                float_row.append(duration)
            stdev = statistics.stdev(float_row)
            y_err.append(stdev)
            y.append(statistics.mean(float_row))
        
        width = 10
        height = 8
        
        plt.figure(figsize=(width, height))
        plt.bar(threads, y)
        plt.title("Philosophers problem: time taken for N threads, avg of 5 runs")
        plt.xlabel("threads")
        plt.ylabel("time taken in milliseconds")
        plt.errorbar(threads, y, y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1, alpha=0.5, ms=4,
                     capsize=2)
        plt.savefig('figure.png', dpi=400, transparent=False)
        plt.show()


if __name__ == "__main__":
    main()
