cd ../demo
ln -sf ../buddy/buddy .
cd ../store
rm -f upkeeper*.sqlite
./schema-setup.sh

cd ../controller
./upk --init

for i in 0 1 2 3 4 5
do
  ./upk --define  package-$i service-$i ../demo/sleep10.sh
  # ./upk --set-desired-state package-$i service-$i start
  # ./upk --set-desired-state package-$i service-$i stop
  # sleep 1
done

echo <<EOF
You'll now need to start a buddy-controller. 
./upkeeper-init is a way to do so.
EOF
