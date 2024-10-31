# Report Paper for the Lab: Best-Offset Prefetching

This folder contains the report.
Build the [report pdf](prefetcher_ow_jk.pdf) with following commands,
```bash
docker pull texlive/texlive:TL2023-historic
docker run -u $UID:$GID --volume ./:/home --rm -it texlive/texlive:TL2023-historic bash -c "cd /home; make"
```
or if `latexmk` is already available on your system, just run the following.
```bash
make
```
