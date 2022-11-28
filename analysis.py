#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
from enum import Enum

import numpy as np


class CurrentProblem(Enum):
    PHILOSOPHERS = "philosophers"
    PHILOSOPHERS_pthread = "pthread_philosophers"
    PRODUCER_CONSUMER = "producer_consumer"
    PRODUCER_CONSUMER_pthread = "pthread_producer_consumer"
    READER_WRITER = "reader_writer"
    READER_WRITER_pthread = "pthread_reader_writer"
    TEST_AND_SET_LOCK = "test_and_set_lock"
    TEST_AND_TEST_AND_SET_LOCK = "test_and_test_and_set_lock"


def plot_file(new_problem, pthread_problem):

    threads = []

    y = []
    y_err = []

    pthread_problem_y = []
    pthread_problem_y_err = []

    with open(f"./data/ingi_server_data/{new_problem}_server.csv") as file:
        reader = csv.reader(file, delimiter=",")
        for row in reader:
            threads.append(int(row[0]))
            float_row = []
            for i in row[1:]:
                time_list = i.split(";")
                duration = float(time_list[0]) - float(time_list[1])
                float_row.append(duration)
            stdev = statistics.stdev(float_row)
            y_err.append(stdev)
            y.append(statistics.mean(float_row))

    with open(f"./data/ingi_server_data/{pthread_problem}_server.csv") as f2:
        reader2 = csv.reader(f2, delimiter=",")
        for row2 in reader2:
            float_row2 = []
            for i2 in row2[1:]:
                time_list = i2.split(";")
                duration = float(time_list[0]) - float(time_list[1])
                float_row2.append(duration)

            stdev = statistics.stdev(float_row2)
            pthread_problem_y_err.append(stdev)
            pthread_problem_y.append(statistics.mean(float_row2))

    width = 10
    height = 8

    bar_width = 0.25

    X_axis = np.array([i for i in range(len(threads))])
    X_labels = [str(i) for i in threads if i % 2 == 0]

    plt.figure(figsize=(width, height))

    plt.bar(X_axis - bar_width / 2, y, bar_width, zorder=3, label=new_problem)
    plt.errorbar(X_axis - bar_width / 2, y, y_err, zorder=4, fmt="none", color="k", errorevery=1, capsize=10, lw=3,
                 capthick=2)

    plt.bar(X_axis + bar_width / 2, pthread_problem_y, bar_width, zorder=3, label=pthread_problem.replace("pthread_",
                                                                                                          "") + " with pthread sync" if new_problem != CurrentProblem.TEST_AND_SET_LOCK.value else pthread_problem.replace(
        "pthread_", ""))
    plt.errorbar(X_axis + bar_width / 2, pthread_problem_y, pthread_problem_y_err, zorder=4, fmt="none", color="k",
                 errorevery=1, capsize=10, capthick=2, lw=3)

    name = new_problem.replace("_", " ")
    if new_problem == CurrentProblem.TEST_AND_SET_LOCK.value:
        name = new_problem.replace("_", " ") + " & test and test and and set"

    plt.xticks(X_axis, X_labels)

    plt.title(f"{name.capitalize()} problem: time taken for N threads, avg of 5 runs")

    plt.xlabel("Number of threads per run")
    plt.ylabel("Time taken in seconds for each run")
    plt.grid(alpha=0.4, zorder=0)
    plt.legend()
    plt.savefig(f'./data/ingi_server_data/figure_{new_problem}_server.png', dpi=400, transparent=False)
    # plt.show()


def main():
    for new_value, pthread_value in [(CurrentProblem.PHILOSOPHERS.value, CurrentProblem.PHILOSOPHERS_pthread.value), (
            CurrentProblem.PRODUCER_CONSUMER.value, CurrentProblem.PRODUCER_CONSUMER_pthread.value),
                                     (CurrentProblem.READER_WRITER.value, CurrentProblem.READER_WRITER_pthread.value), (
                                             CurrentProblem.TEST_AND_SET_LOCK.value,
                                             CurrentProblem.TEST_AND_TEST_AND_SET_LOCK.value)]:
        plot_file(new_value, pthread_value)


if __name__ == "__main__":
    main()
