<?php
  // Get the values of the query string parameters
  $name = $_GET['name'];
  $age = $_GET['age'];
  $occupation = $_GET['occupation'];

  // Display the retrieved values
  echo "Name: $name<br>";
  echo "Age: $age<br>";
  echo "Occupation: $occupation<br>";
?>
