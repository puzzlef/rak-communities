#!/usr/bin/env bash
src="rak-communities-seq"
out="/home/resources/Documents/subhajit/$src.log"
ulimit -c unlimited
ulimit -s unlimited
printf "" > "$out"

# Download program
rm -rf $src
git clone https://github.com/puzzlef/$src
cd $src

# Run
g++ -std=c++17 -O3 main.cxx
stdbuf --output=L ./a.out ~/data/GAP-urand.mtx      1 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/GAP-kron.mtx       1 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/GAP-web.mtx        0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/GAP-twitter.mtx    0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/GAP-road.mtx       1 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/com-Friendster.mtx 1 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/com-Orkut.mtx      1 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/sk-2005.mtx        0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/it-2004.mtx        0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/webbase-2001.mtx   0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/uk-2005.mtx        0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/arabic-2005.mtx    0 2>&1 | tee -a "$out"
stdbuf --output=L ./a.out ~/data/kmer_V1r.mtx       1 2>&1 | tee -a "$out"
