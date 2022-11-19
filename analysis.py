#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
import numpy as np
from enum import Enum


class CurrentProblem(Enum):
    PHILOSOPHERS = "philosophers"
    READER_WRITER = "reader/writer"
    PRODUCER_CONSUMER = "producer/consumer"


def main():
    with open("mesures.csv") as file:
        reader = csv.reader(file, delimiter=",")

        current_problem: CurrentProblem = CurrentProblem.PHILOSOPHERS

        x = []
        y = []
        y_err = []

        for row in reader:
            if row[0].find(CurrentProblem.PHILOSOPHERS.value):
                current_problem = CurrentProblem.PHILOSOPHERS
            elif row[0].find(CurrentProblem.READER_WRITER.value):
                current_problem = CurrentProblem.READER_WRITER
            elif row[0].find(CurrentProblem.PRODUCER_CONSUMER.value):
                current_problem = CurrentProblem.PRODUCER_CONSUMER
            else:
                x.append(row[0])

        # x = [1, 2, 4, 8]
        # y = []
        #
        # y_err = []
        # for row in reader:
        #     float_row = [float(i) for i in row]
        #     stdev = statistics.stdev(float_row[1:])
        #     y_err.append(stdev)
        #     y.append(statistics.mean(float_row[1:]))
        #
        # width = 10
        # height = 8
        #
        # plt.figure(figsize=(width, height))
        # plt.bar(x, y)
        # plt.title("Time taken per thread, avg of 5 runs")
        # plt.xlabel("threads")
        # plt.ylabel("time taken in seconds")
        # plt.errorbar(x, y, y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1, alpha=0.5, ms=4,
        #              capsize=2)
        # plt.savefig('figure.png', dpi=400, transparent=True)
        # plt.show()


if __name__ == "__main__":
    main()
