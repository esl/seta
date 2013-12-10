while true; do
     for ((N = 1; N <= 20; N++)); do
         echo "running scheduler to evaluate fib($N)..."
         make FIB="$N" run
         sleep 3
         echo ""
     done
done
