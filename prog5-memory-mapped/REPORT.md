# Program 5: Memory-Mapped Files
* Name:		Michael Bruno
* Version:	85%
* Time Spent:	6 hours

## Test Transcripts
### reversemm.c
make test-mm
hexdump med_test | head -n 5
0000000 abb5 5ece 2668 0e51 89fb f775 8d11 8b79
0000010 720b bf46 13b5 dc24 37d5 edd6 730a 2644
0000020 5125 81de a7bc e12a 8171 0634 4ac2 c98a
0000030 cf6f 51a1 4e70 ea13 4e97 20eb a738 fe43
0000040 62b0 92c1 902e 6149 a3ed df01 bc03 3669
./reversemm med_test
hexdump med_test | head -n 5
0000000 5c1c 88aa bee6 3355 c8f6 9f4a 2711 55e0
0000010 c1ae fb89 c403 d67c c5b6 09c0 d03a 1fed
0000020 f312 0b68 279b 88b2 fcfc f3a8 5341 3266
0000030 bb6b 7535 a482 ec4b dd1f 53db fe23 51f6
0000040 0a74 8a6a 1c7b 1567 307d d170 868f 8a19
./reversemm med_test
hexdump med_test | head -n 5
0000000 abb5 5ece 2668 0e51 89fb f775 8d11 8b79
0000010 720b bf46 13b5 dc24 37d5 edd6 730a 2644
0000020 5125 81de a7bc e12a 8171 0634 4ac2 c98a
0000030 cf6f 51a1 4e70 ea13 4e97 20eb a738 fe43
0000040 62b0 92c1 902e 6149 a3ed df01 bc03 3669

### reversecio.c
make test-cio
hexdump med_test | head -n 5
0000000 abb5 5ece 2668 0e51 89fb f775 8d11 8b79
0000010 720b bf46 13b5 dc24 37d5 edd6 730a 2644
0000020 5125 81de a7bc e12a 8171 0634 4ac2 c98a
0000030 cf6f 51a1 4e70 ea13 4e97 20eb a738 fe43
0000040 62b0 92c1 902e 6149 a3ed df01 bc03 3669
./reversecio med_test
hexdump med_test | head -n 5
0000000 5c1c 88aa bee6 3355 c8f6 9f4a 2711 55e0
0000010 c1ae fb89 c403 d67c c5b6 09c0 d03a 1fed
0000020 f312 0b68 279b 88b2 fcfc f3a8 5341 3266
0000030 bb6b 7535 a482 ec4b dd1f 53db fe23 51f6
0000040 0a74 8a6a 1c7b 1567 307d d170 868f 8a19
./reversecio med_test
hexdump med_test | head -n 5
0000000 abb5 5ece 2668 0e51 89fb f775 8d11 8b79
0000010 720b bf46 13b5 dc24 37d5 edd6 730a 2644
0000020 5125 81de a7bc e12a 8171 0634 4ac2 c98a
0000030 cf6f 51a1 4e70 ea13 4e97 20eb a738 fe43
0000040 62b0 92c1 902e 6149 a3ed df01 bc03 3669

## Program Timings
(1) reversemm	med_test: 0.73s
(1) reversecio	med_test: 0.71s

(2) reversemm	med_test: 0.72s
(2) reversecio	med_test: 0.72s

(3) reversemm	med_test: 0.72s
(3) reversecio	med_test: 0.71s

reversemm average: 	0.72s
reversecio average:	0.713s

## Academic Integrity Statement
* I verify that I have recieved no help from anyone other than the help indicated.
