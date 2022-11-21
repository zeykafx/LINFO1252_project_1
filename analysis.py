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


def plot_file(file, current_problem: str):
    reader = csv.reader(file, delimiter=",")

    threads = []
    y = []
    
    y_err = []
    for row in reader:
        threads.append(row[0])
        float_row = []
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
    name = current_problem.replace("_", " ")
    plt.title(f"{name} problem: time taken for N threads, avg of 5 runs")
    plt.xlabel("Number of threads per run")
    plt.ylabel("Time taken in milliseconds for each run")
    plt.errorbar(threads, y, y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1, alpha=0.5, ms=4,
                    capsize=2)
    plt.savefig(f'figure_{current_problem}.png', dpi=400, transparent=False)
    plt.show()


def main():
    for i in [CurrentProblem.PHILOSOPHERS.value, CurrentProblem.PRODUCER_CONSUMER.value, CurrentProblem.READER_WRITER.value]:
        with open(f"{i}.csv") as file:
            plot_file(file, i)


if __name__ == "__main__":
    main()
