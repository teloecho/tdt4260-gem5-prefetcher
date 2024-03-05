#https://www.gem5.org/documentation/general_docs/building
#docker pull ghcr.io/gem5/ubuntu-22.04_all-dependencies:v23-0
#docker run -u $UID:$GID --volume ~/tdt4260-ca-gem5:/gem5 --rm -it ghcr.io/gem5/ubuntu-22.04_all-dependencies:v23-0
# for rootless docker:
docker run --volume ~/tdt4260-gem5-prefetcher:/gem5 --rm -it ghcr.io/gem5/ubuntu-22.04_all-dependencies:v23-0
#cd gem5