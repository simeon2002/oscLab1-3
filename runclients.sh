#!/bin/bash

# Run client targets in separate terminals
gnome-terminal -- bash -c "make runclient1; exec bash" &
gnome-terminal -- bash -c "make runclient2; exec bash" &
gnome-terminal -- bash -c "make runclient3; exec bash" &
