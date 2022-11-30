#!/usr/bin/python3
import csv
import statistics

import matplotlib.pyplot as plt
from enum import Enum

import numpy as np

USING_INGI_DATA = False
file_path = "./data/"
if USING_INGI_DATA:
    file_path += "ingi_server_data/"


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

    extension = ""
    if USING_INGI_DATA:
        extension += "_server"
    with open(f"{file_path}{new_problem}{extension}.csv") as file:
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

    with open(f"{file_path}{pthread_problem}{extension}.csv") as f2:
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

    plt.plot(X_axis, y, 'o-', label=new_problem, color='#CC4F1B')
    plt.fill_between(X_axis, np.subtract(y, y_err), np.add(y, y_err), alpha=0.2, edgecolor='#CC4F1B', facecolor='#FF9848')

    plt.plot(X_axis, pthread_problem_y, 'o-', color='#1B2ACC', label=pthread_problem.replace("pthread_", "") + " with pthread sync" if new_problem != CurrentProblem.TEST_AND_SET_LOCK.value else pthread_problem.replace("pthread_", ""))
    plt.fill_between(X_axis, np.subtract(pthread_problem_y, pthread_problem_y_err), np.add(pthread_problem_y, pthread_problem_y_err), alpha=0.2, edgecolor='#1B2ACC', facecolor='#089FFF')
    name = new_problem.replace("_", " ")
    if new_problem == CurrentProblem.TEST_AND_SET_LOCK.value:
        name = new_problem.replace("_", " ") + " & test and test and and set"

    plt.xticks(X_axis, X_labels)

    plt.title(f"{name.capitalize()} problem, avg of 5 runs")

    plt.xlabel("Number of threads")
    plt.ylabel("Execution time (seconds)")
    plt.grid(alpha=0.4, zorder=0)
    plt.legend()
    plt.savefig(f'{file_path}figure_{new_problem}.png', dpi=400, transparent=False)
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
