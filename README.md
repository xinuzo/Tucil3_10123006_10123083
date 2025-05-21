# Penyelesaian Puzzle Rush Hour Menggunakan Algoritma Pathfinding

Rush Hour adalah sebuah permainan puzzle logika berbasis grid yang menantang pemain untuk menggeser kendaraan di dalam sebuah kotak (biasanya berukuran 6x6) agar mobil utama (biasanya berwarna merah) dapat keluar dari kemacetan melalui pintu keluar di sisi papan. Setiap kendaraan hanya bisa bergerak lurus ke depan atau ke belakang sesuai dengan orientasinya (horizontal atau vertikal), dan tidak dapat berputar. Tujuan utama dari permainan ini adalah memindahkan mobil merah ke pintu keluar dengan jumlah langkah seminimal mungkin. 


## Instalasi & Penggunaan

### Prasyarat
- g++ compiler untuk C++

### Cara Menjalankan Program
**Clone Repository**
```bash
git clone https://github.com/xinuzo/Tucil3_10123006_10123083.git
cd Tucil3_10123006_10123083/src
```
**Kompilasi Program**
```bash
g++ src/rush_hour_solver.cpp -o bin/app.o
```

## Masukan
Program akan meminta input berikut dari sebuah berkas .txt:
A B
N
konfigurasi_papan
dengan kemungkinan konfigurasi papan:
K
XXX
XXX
XXX
Contoh: tcase1.txt


## Keluaran
Program akan menghasilkan berkas berisikan langkah-langkah primary piece menuju pintu keluar seperti sebagai berikut  
Papan Awal
AAB..F
..BCDF
GPPCDFK
GH.III
GHJ...
LLJMM.

Gerakan 1: I-kiri
AAB..F
..BCDF
GPPCDFK
GHIII.
GHJ...
LLJMM.

Gerakan 2: F-bawah
AAB...
..BCD.
GPPCD.K
GHIIIF
GHJ..F
LLJMMF

Gerakan 3: D-atas
AAB.D.
..BCD.
GPPC..K
GHIIIF
GHJ..F
LLJMMF

Gerakan 4: C-atas
AABCD.
..BCD.
GPP...K
GHIIIF
GHJ..F
LLJMMF

Gerakan 5: P-kanan
AABCD.
..BCD.
G...PPK
GHIIIF
GHJ..F
LLJMMF

Waktu (ms): 26
---

## Author 
Rendi Adinata (10123083) | Muhammad Naufal Rayhannida (1023006)
Kelas: K1

