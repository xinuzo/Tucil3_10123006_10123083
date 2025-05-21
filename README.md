# Penyelesaian Puzzle Rush Hour Menggunakan Algoritma Pathfinding

## Deskripsi
Rush Hour adalah permainan puzzle logika berbasis grid yang menantang pemain untuk menggeser kendaraan agar mobil utama (P) dapat keluar melalui pintu keluar (K). Setiap kendaraan hanya bisa bergerak lurus sesuai orientasinya (horizontal/vertikal).

## Instalasi & Penggunaan

### Prasyarat
- g++ compiler untuk C++

### Langkah-langkah
1. **Clone Repository**
   ```bash
   git clone https://github.com/xinuzo/Tucil3_10123006_10123083.git
   cd Tucil3_10123006_10123083/src
   ```
2. **Kompilasi Program**
   ```bash
   g++ rush_hour_solver.cpp -o rush_hour
   ```
3. **Jalankan Program**
   ```bash
   ./rush_hour
   ```
   Masukkan path file input ketika diminta (contoh: `input/tcase1.txt`)

## Format Input
File `.txt` dengan struktur:
```
A B
N
[baris 1]
[baris 2]
...
[baris A]
```

Contoh (`tcase1.txt`):
```
6 6
12
AAB..F
..BCDF
GPPCDFK
GH.III
GHJ...
LLJMM.
```

## Format Output
Program akan menghasilkan:
1. Papan awal
2. Urutan langkah solusi dengan perubahan papan tiap langkah
3. Waktu eksekusi

Contoh Output:
```
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
```

## Pembuat
- Rendi Adinata (10123083)
- Muhammad Naufal Rayhannida (10123006)  
Kelas K1

