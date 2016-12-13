#!/bin/sh


echo "getting output for list_lock"
for i in 2 4; do
  echo "with $i threads..."
  echo "samples to skip = 50"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_list_lock $i 50 > ./50/randtrack-list-lock$i-$j.cmp
    sort -n ./50/randtrack-list-lock$i-$j.cmp > ./50/randtrack-list-lock$i-$j.cmps
    diff ./50/randtrack-list-lock$i-$j.cmps ./50/randtrack-orig1-$j.cmps
  done
  echo "samples to skip = 100"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_list_lock $i 100 > ./100/randtrack-list-lock$i-$j.cmp
    sort -n ./100/randtrack-list-lock$i-$j.cmp > ./100/randtrack-list-lock$i-$j.cmps
    diff ./100/randtrack-list-lock$i-$j.cmps ./100/randtrack-orig1-$j.cmps
  done
done

echo "getting output for element_lock"
for i in 1 2 4; do
  echo "with $i threads..."
  echo "samples to skip = 50"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_element_lock $i 50 > ./50/randtrack-element-lock$i-$j.cmp
    sort -n ./50/randtrack-element-lock$i-$j.cmp > ./50/randtrack-element-lock$i-$j.cmps
    diff ./50/randtrack-element-lock$i-$j.cmps ./50/randtrack-orig1-$j.cmps
  done
  echo "samples to skip = 100"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_element_lock $i 100 > ./100/randtrack-element-lock$i-$j.cmp
    sort -n ./100/randtrack-element-lock$i-$j.cmp > ./100/randtrack-element-lock$i-$j.cmps
    diff ./100/randtrack-element-lock$i-$j.cmps ./100/randtrack-orig1-$j.cmps
  done
done

echo "getting output for reduction"
for i in 1 2 4; do
  echo "with $i threads..."
  echo "samples to skip = 50"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_reduction $i 50 > ./50/randtrack-reduction$i-$j.cmp
    sort -n ./50/randtrack-reduction$i-$j.cmp > ./50/randtrack-reduction$i-$j.cmps
    diff ./50/randtrack-reduction$i-$j.cmps ./50/randtrack-orig1-$j.cmps
  done
  echo "samples to skip = 100"
  for j in 1 2 3 4 5; do
    /usr/bin/time -p ./randtrack_reduction $i 100 > ./100/randtrack-reduction$i-$j.cmp
    sort -n ./100/randtrack-reduction$i-$j.cmp > ./100/randtrack-reduction$i-$j.cmps
    diff ./100/randtrack-reduction$i-$j.cmps ./100/randtrack-orig1-$j.cmps
  done
done

