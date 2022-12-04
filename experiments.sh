#!/usr/bin/bash

DATA_FOLDER="./data"

make -s clean &>/dev/null
make -s &>/dev/null

# you can call the script with `false` to test the program with our sync primitives, if you call the program with `true` the script will test the pthread sync primitives
USING_PTHREAD=$1

# delete old perf data
rm "$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")philosophers.csv"
rm "$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")reader_writer.csv"
rm "$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")producer_consumer.csv"
rm "$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")test_and_set_lock.csv"
rm "$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")test_and_test_and_set_lock.csv"

threads=(2 4 8 16 32 64)

echo "$( ([[ $USING_PTHREAD = true ]] && echo "Running with pthread sync") || echo "Running with own sync")"

# Loop over all the arguments
for pgm in N l t p,c w,r; do
  IFS=","
  set -- $pgm
  # use $1 for the first element and $2 for the second (if applicable)

  current_problem=$([[ $1 = "N" ]] && echo "philosophers" || ([[ $1 = "w" ]] && echo "reader_writer" || ([[ $1 = "l" ]] && echo "test_and_set_lock" || ([[ $1 = "t" ]] && echo "test_and_test_and_set_lock" || echo "producer_consumer"))))
  echo "Running $current_problem"
  # echo "$current_problem" >>$FILENAME
  FILENAME="$DATA_FOLDER/$([[ $USING_PTHREAD = true ]] && echo "pthread_")$current_problem.csv"

  for t in "${threads[@]}"; do
    NUM_THREADS=0

    # If this is P, l or t, it means that the script will run the philosophers, lock, or test and set program, so we dont want to divide the number of threads.
    if [ "$1" = "N" ] || [[ "$1" = "l" ]] || [[ "$1" = "t" ]]; then
      NUM_THREADS=$t
      if [ "$NUM_THREADS" -lt 2 ]; then
        continue
      fi
    else
      # Otherwise, we are either going to run the producer/consumer or the reader/writer problem and we need to divide the number of threads in 2
      NUM_THREADS=$((t / 2))

    fi

    if [ $NUM_THREADS -ne 0 ]; then
      echo "$t," >>$FILENAME
      truncate -s -1 $FILENAME # removes \n added just above
      for _ in {1..5}; do

        # runs the binary and times it at the same time

        # basically, the nested ternary operation just checks if there is a need for a second argument (if we aren't running the philosophers, lock, or test and set program )
        # if that's the case, then we return the number of thread for that argument
        # -> The second arg is always the consumer/reader (because of the outer for loop)
        START="$(date +%s.%N)"
        ./bin/binary "$([[ $USING_PTHREAD = true ]] && echo "-o")" -"$1" $NUM_THREADS "$( ([[ "$1" != "N" ]] && [[ "$1" != "l" ]] && [[ "$1" != "t" ]]) && echo "-$2 $NUM_THREADS")"
        echo "$(date +%s.%N);$START," >>$FILENAME # we dont compute the time taken in the bash script, instead we let the python script compute the difference.

        truncate -s -1 $FILENAME # removes the \n added above
      done
      truncate -s -1 $FILENAME # removes the trailing ","
      echo >>$FILENAME         # appends a newline
    fi

  done
done
