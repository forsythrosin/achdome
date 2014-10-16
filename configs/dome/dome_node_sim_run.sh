#!/usr/bin/env bash

./Achdome -config configs/dome/dome_node_sim.xml -local 1 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 2 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 3 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 4 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 5 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 6 --slave &
./Achdome -config configs/dome/dome_node_sim.xml -local 0
