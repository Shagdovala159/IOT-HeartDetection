<?php
// Koneksi ke database
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "tugas_akhir";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Koneksi gagal: " . $conn->connect_error);
}

// Mendapatkan data pasien berdasarkan kode pasien
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    // Mendapatkan kode pasien dari parameter URL
    $kodePasien = $_GET['kodepasien'];

    // Membuat query SELECT
    $sql = "SELECT * FROM datapasien WHERE kodepasien = '$kodePasien'";
    $result = $conn->query($sql);

    // Memeriksa apakah data ditemukan
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $dataPasien = array(
            'id' => $row['id'],
            'kodepasien' => $row['kodepasien'],
            'nama' => $row['nama'],
            'usia' => $row['usia'],
            'detakjantung' => $row['detakjantung']
        );

        // Mengubah data menjadi format JSON dan mengirimkan sebagai response
        header('Content-Type: application/json');
        echo json_encode($dataPasien);
    } else {
        // Jika data tidak ditemukan
        header('Content-Type: application/json');
        echo json_encode(array('message' => 'Data pasien tidak ditemukan.'));
    }
} elseif ($_SERVER['REQUEST_METHOD'] === 'PUT') {
    // Menerima data dari payload
    $data = json_decode(file_get_contents('php://input'), true);

    // Mendapatkan kode pasien dan detak jantung dari payload
    $kodePasien = $data['kodepasien'];
    $detakJantung = $data['detakjantung'];

    // Membuat query UPDATE untuk memperbarui detak jantung pasien
    $sql = "UPDATE datapasien SET detakjantung = '$detakJantung' WHERE kodepasien = '$kodePasien'";
    $result = $conn->query($sql);

    // Memeriksa apakah data berhasil diperbarui
    if ($conn->affected_rows > 0) {
        header('Content-Type: application/json');
        echo json_encode(array('message' => 'Detak jantung berhasil diperbarui.'));
    } else {
        header('Content-Type: application/json');
        echo json_encode(array('message' => 'Gagal memperbarui detak jantung.'));
    }
}

$conn->close();
?>
