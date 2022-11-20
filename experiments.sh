#!/usr/bin/bash

make -s clean &>/dev/null
make -s &>/dev/null
rm ./philosophers.csv
rm ./reader_writer.csv
rm ./producer_consumer.csv
# FILENAME=./philosophers.csv

CORE_COUNT=$(grep ^cpu\\scores /proc/cpuinfo | uniq | awk '{print $4}')
MAX_THREADS=$((2 * "$CORE_COUNT"))
threads=($(seq $MAX_THREADS))

# Loop over all the arguments
for pgm in N p,c w,r; do
  IFS=","
  set -- $pgm
  # use $1 for the first element and $2 for the second (if applicable)

  current_problem=$([[ $1 = "N" ]] && echo "philosophers" || ([[ $1 = "w" ]] && echo "reader_writer" || echo "producer_consumer"))
  echo "Running $current_problem"
  # echo "$current_problem" >>$FILENAME
  FILENAME="./$current_problem.csv"

  for t in "${threads[@]}"; do
    NUM_THREADS=0
    IS_EVEN=true

    # If this is P, it means we'll run the philosophers program, so we dont need to divide the number of threads.
    if [ "$1" = "N" ]; then
      NUM_THREADS=$t
      if [ "$NUM_THREADS" -lt 2 ]; then
        continue
      fi
    else
      # Otherwise, we are either going to run the producer/consumer or the reader/writer problem and we need to divide the number of threads in 2
      NUM_THREADS=$((t / 2))

      if [ $((t % 2)) -eq 0 ]; then
        # if t is pair, then assign the same number of threads to both arguments
        IS_EVEN=true
      else
        # if its odd, then we'll add one more thread to the reader/writer
        IS_EVEN=false
      fi

    fi

    if [ $NUM_THREADS -ne 0 ]; then
      echo "$t," >>$FILENAME
      truncate -s -1 $FILENAME # removes \n the added just above
      for _ in {1..5}; do

        # runs the binary and timing it at the same time
        # basically, the nested ternary operation just checks if there is a need for a second argument (if we aren't running the philosophers problem)
        # if that's the case, and the number of thread is even, then we return the number of thread for that argument, otherwise we add one to the number of thread and return that sum
        # -> The second arg is always the consumer/reader (because of the outer for loop)
        START="$(date +%s.%N)"
        ./bin/binary -"$1" $NUM_THREADS "$([[ "$1" != "N" ]] && ([[ "$IS_EVEN" = true ]] && echo "-$2 $NUM_THREADS" || echo "-$2 $((NUM_THREADS + 1))"))"
        # DURATION=$(echo "$(date +%s.%N) - $START" | bc -l)
        echo "$(date +%s.%N);$START," >>$FILENAME

        truncate -s -1 $FILENAME # removes the \n added by time
      done
      truncate -s -1 $FILENAME # removes the trailing ","
      echo >>$FILENAME         # appends a newline
    fi

  done
done