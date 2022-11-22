#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
from enum import Enum

import numpy as np


class CurrentProblem(Enum):
    PHILOSOPHERS = "philosophers"
    READER_WRITER = "reader_writer"
    PRODUCER_CONSUMER = "producer_consumer"
    TEST_AND_SET_LOCK = "test_and_set_lock"
    TEST_AND_TEST_AND_SET_LOCK = "test_and_test_and_set_lock"


def plot_file(file, current_problem: str):
    reader = csv.reader(file, delimiter=",")

    threads = []
    y = []
    y_err = []
    ttas_y = []
    ttas_y_err = []

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

    if current_problem == CurrentProblem.TEST_AND_SET_LOCK.value:
        with open(f"./data/{CurrentProblem.TEST_AND_TEST_AND_SET_LOCK.value}.csv") as f2:
            reader2 = csv.reader(f2, delimiter=",")
            for row2 in reader2:
                float_row2 = []
                for i2 in row2[1:]:
                    time_list = i2.split(";")
                    duration = float(time_list[0]) - float(time_list[1])
                    float_row2.append(duration)

                stdev = statistics.stdev(float_row2)
                ttas_y_err.append(stdev)
                ttas_y.append(statistics.mean(float_row2))

    width = 10
    height = 8

    X_axis = np.arange(len(threads)) + 0.2 if current_problem == CurrentProblem.TEST_AND_SET_LOCK.value else threads

    plt.figure(figsize=(width, height))

    plt.bar(X_axis, y, 0.4, label="test and set")
    plt.errorbar(X_axis, y, y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1, alpha=0.5, ms=4,
                 capsize=2)
    if current_problem == CurrentProblem.TEST_AND_SET_LOCK.value:
        plt.bar(X_axis - 0.4, ttas_y, 0.4, label="test and test and and set")
        plt.errorbar(X_axis - 0.4, ttas_y, ttas_y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1,
                     alpha=0.5, ms=4,
                     capsize=2)

    name = current_problem.replace("_", " ")
    plt.title(f"{name} problem: time taken for N threads, avg of 5 runs")
    plt.xlabel("Number of threads per run")
    plt.ylabel("Time taken in milliseconds for each run")

    plt.savefig(f'./data/figure_{current_problem}.png', dpi=400, transparent=False)
    # plt.show()


def main():
    for i in [CurrentProblem.PHILOSOPHERS.value, CurrentProblem.PRODUCER_CONSUMER.value,
              CurrentProblem.READER_WRITER.value, CurrentProblem.TEST_AND_SET_LOCK.value]:
        with open(f"./data/{i}.csv") as file:
            plot_file(file, i)


if __name__ == "__main__":
    main()
