for i in {1..3}; do
  cat /dev/cu.usbmodem2101 &
  pid=$!
  sleep 1
  kill $pid 2>/dev/null
done
