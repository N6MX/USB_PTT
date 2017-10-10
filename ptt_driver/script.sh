#!/bin/bash

while [ 1 ];do
./query_app -rs
sleep 0.2;
./query_app -rc
sleep 0.5;
done;
