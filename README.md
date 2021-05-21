# soal-shift-sisop-modul-3-F10-2021

<br>

# **Nomor 1**

Untuk menghandle berbagai request dari client, berikut adalah gambaran besar bagaimana server & client berkomunikasi:

<br>

### **Inisialisasi koneksi antara server & client**

![Main](./img/soal1/Main.png)

<br>

### **Handle request**

![Handle](./img/soal1/Handle.png)

- Disini client akan menginputkan request utamanya, lalu dikirim ke server dan dilakukan _handling request_ sesuai input requestnya.

- Pada client, `extract_request()` dibutuhkan untuk request:
  - DELETE
  - DOWNLOAD
  - FIND

<br>

## **a. REGISTER & LOGIN**

![Code](./img/soal1/Register.png)

### Penjelasan:

1. Client input **id** dan **password**

2. Kirim **id** dan **password** ke server

3. Server mencatat **id** dan **password** ke file **akun.txt**

<br>

## **b. Database files.tsv & folder FILES**

- Pembuatan folder **FILES** telah dilakukan pada fungsi `main()` server yaitu dengan fungsi `mkdir()`.

- Pembuatan serta pencatatan **files.tsv** akan dilakukan bersamaan pada fitur **ADD** dan **DELETE** (lihat poin c dan poin e).

<br>

## **c. ADD file ke server**

![Code](./img/soal1/Add-1.png)

![Code](./img/soal1/Add-2.png)

![Code](./img/soal1/Add-3.png)

### Penjelasan:

1. Server mengirim status login kepada client, jika client belum login maka **request ADD** dibatalkan.

2. Client menginputkan data berupa:

   - Publisher
   - Tahun publishing
   - Filepath pada komputer client yang mengarah ke file yang ingin diupload

3. Client mengirim status apakah file tersebut ada / tidak. Jika tidak maka proses selesai.
4. Client mengirim informasi file (publisher, tahun, nama file) ke server
5. Client mengirim ukuran file ke server
6. Kirim file dengan teknik looping. Data yang dikirimkan adalah data binary. Teknik looping dibutuhkan karena data yang dikirimkan tidak bisa langsung sekaligus (misal 1GB), jadinya akan dikirim 1 Bytes lalu 1 Bytes lalu 1 Bytes... sampai selesai.
7. Server menerima potongan-potongan kecil data lalu menyimpannya kedalam folder **FILES**. Ini dilakukan hingga potongan-potongan tersebut selesai dikirim.
8. Server mencatat **LOG** untuk **ADD FILES**. Proses selesai.

| Lihat proses mencatat log pada **poin h**

<br>

## **d. DOWNLOAD file dari server ke client**

![Code](./img/soal1/Download-1.png)

![Code](./img/soal1/Download-2.png)

### Penjelasan:

1. Server mengirim status login kepada client, jika client belum login maka **request DOWNLOAD** dibatalkan.

2. Client mengirimkan nama file ke server.
3. Server melakukan pengecekan ketersediaan file yang diminta client dengan melihat pada **files.tsv**.
4. Server mengirim status ketersediaan file yang diminta client.
5. Server mengirim ukuran file yang diminta ke client
6. Kirim file dengan teknik looping. Data yang dikirimkan adalah data binary. Teknik looping dibutuhkan karena data yang dikirimkan tidak bisa langsung sekaligus (misal 1GB), jadinya akan dikirim 1 Bytes lalu 1 Bytes lalu 1 Bytes... sampai selesai.
7. Client menerima potongan-potongan kecil data lalu menyimpannya kedalam folder **Client**. Ini dilakukan hingga potongan-potongan tersebut selesai dikirim.
8. Proses selesai.

<br>

## **e. DELETE file pada server**

![Code](./img/soal1/Delete-1.png)

![Code](./img/soal1/Delete-2.png)

### Penjelasan:

Ide dari penghapusan baris pada database **files.tsv** adalah dengan membuat file database baru lalu meng-copy setiap baris pada database lama **kecuali** data file yang ingin dihapus. Jika file yang ingin dihapus itu ada, maka file database lama akan dihapus.

1. Server mengirim status login kepada client, jika client belum login maka **request DELETE** dibatalkan.

2. Client mengirimkan nama file yang ingin dihapus pada server
3. Server melakukan copy database dengan mengiterasi setiap baris pada **files.tsv**. Jika ditemukan data file yang ingin dihapus maka tidak akan dicopy ke database baru kemudian tandai bahwa file yang ingin dihapus itu ada.
4. Server mengirim status apakah file yang ingin dihapus ada atau tidak.
5. Jika file tidak ada, maka hapus database baru, proses selesai.
6. Hapus database lama
7. Server mencatat **LOG** untuk **DELETE FILES**. Proses selesai.

| Lihat proses mencatat log pada **poin h**

<br>

## **f. SEE file pada server**

## **g. FIND file pada server**

![Code](./img/soal1/Search-1.png)

![Code](./img/soal1/Search-2.png)

### Penjelasan:

Request **SEE** dan **FIND** sebenarnya bekerja hampir sama. Bedanya request **FIND** hanya menampilkan data nama files yang memiliki kesesuaian dengan keyword. Oleh karena itu akan dijelaskan sekaligus

1. Server mengirim status login kepada client, jika client belum login maka **request SEE** dibatalkan.

2. Jika request yang diterima adalah **FIND** maka variabel `do_filter` akan bernilai `true`. Artinya nanti saat sebelum menampilkan informasi ke client akan dicek terlebih dahulu apakah file mengandung keyword pencarian atau tidak. Jika tidak maka tidak akan ditampilkan.

3. Server mengirim setiap baris pada database **files.tsv** ke client dengan teknik looping.

4. Client menerima baris data lalu menentukan apakah perlu dilakukan filter atau tidak untuk keputusan penampilan informasi.

5. Tampilkan data pada client sesuai format pada soal

<br>

## **h. Pencatatan LOG**

- Pencatatan telah dilakuakan bersamaan untuk setiap request **ADD** dan **DELETE**.
- Hal tersebut dilakukan dengan fungsi `record_log(mode, nama_files)` [lihat fungsi disini](https://github.com/DewanggaDh/soal-shift-sisop-modul-3-F10-2021/blob/main/soal1/Server/server.c#L484)
- Pada dasarnya yang dilakukan adalah mencatat informasi berdasarkan mode kedalam file bernama **running.log**

<br><br>
