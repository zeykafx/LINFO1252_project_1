#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
from enum import Enum

import numpy as np


class CurrentProblem(Enum):
    PHILOSOPHERS = "philosophers"
    PHILOSOPHERS_OLD = "old_philosophers"
    READER_WRITER = "reader_writer"
    READER_WRITER_OLD = "old_reader_writer"
    PRODUCER_CONSUMER = "producer_consumer"
    PRODUCER_CONSUMER_OLD = "old_producer_consumer"
    TEST_AND_SET_LOCK = "test_and_set_lock"
    TEST_AND_TEST_AND_SET_LOCK = "test_and_test_and_set_lock"


def plot_file(file, current_problems: (str, str)):
    new_problem, old_problem = current_problems

    reader = csv.reader(file, delimiter=",")

    threads = []
    y = []
    y_err = []

    old_problem_y = []
    old_problem_y_err = []

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

    with open(f"./data/{old_problem}.csv") as f2:
        reader2 = csv.reader(f2, delimiter=",")
        for row2 in reader2:
            float_row2 = []
            for i2 in row2[1:]:
                time_list = i2.split(";")
                duration = float(time_list[0]) - float(time_list[1])
                float_row2.append(duration)

            stdev = statistics.stdev(float_row2)
            old_problem_y_err.append(stdev)
            old_problem_y.append(statistics.mean(float_row2))

    width = 10
    height = 8

    X_axis = np.arange(len(threads)) + 0.2

    plt.figure(figsize=(width, height))

    plt.bar(X_axis, y, 0.4, label=new_problem)
    plt.errorbar(X_axis, y, y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1, alpha=0.5, ms=4,
                 capsize=2)

    plt.bar(X_axis - 0.4, old_problem_y, 0.4, label=old_problem.replace("old_", "") + " with pthread sync")
    plt.errorbar(X_axis - 0.4, old_problem_y, old_problem_y_err, fmt=".", color="Black", elinewidth=2, capthick=10, errorevery=1,
                 alpha=0.5, ms=4,
                 capsize=2)

    name = new_problem.replace("_", " ")
    if new_problem == CurrentProblem.TEST_AND_SET_LOCK.value:
        name = new_problem.replace("_", " ") + " & test and test and and set"

    plt.title(f"{name} problem: time taken for N threads, avg of 5 runs")

    plt.xlabel("Number of threads per run")
    plt.ylabel("Time taken in seconds for each run")
    plt.legend()
    plt.savefig(f'./data/figure_{new_problem}.png', dpi=400, transparent=False)
    # plt.show()


def main():
    for i in [(CurrentProblem.PHILOSOPHERS.value, CurrentProblem.PHILOSOPHERS_OLD.value), (CurrentProblem.PRODUCER_CONSUMER.value, CurrentProblem.PRODUCER_CONSUMER_OLD.value),
              (CurrentProblem.READER_WRITER.value, CurrentProblem.READER_WRITER_OLD.value), (CurrentProblem.TEST_AND_SET_LOCK.value, CurrentProblem.TEST_AND_TEST_AND_SET_LOCK.value)]:
        with open(f"./data/{i[0]}.csv") as file:
            plot_file(file, i)


if __name__ == "__main__":
    main()
