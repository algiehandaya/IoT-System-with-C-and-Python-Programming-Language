<?php
if($_SERVER['REQUEST_METHOD'] == 'POST'){
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "smaq2842_smartfishery";
    
    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
    }

    $idtambak = $_POST['idtambak'];
    $peha = $_POST['peha'];
    $suhu = $_POST['suhu'];
    $jenisikan = $_POST['jenisikan'];

    //Debug
    //echo $idtambak ; return;

    $sql = "INSERT INTO ikan (id_tambak, ph, suhu, jenis_ikan)
    VALUES ('$idtambak', '$peha', '$suhu', '$jenisikan')";
    
    if ($conn->query($sql) === TRUE) {
      echo "New record created successfully";
    } else {
      echo "Error: " . $sql . "<br>" . $conn->error;
    }

    $conn->close();
  }
?>
