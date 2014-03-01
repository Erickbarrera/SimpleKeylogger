<?php
    require_once('include.php');

    // check if whoever is viewing this file is authenticated
    if ( $_SESSION[LOGIN_TOKEN] != "VALID")
    {
        header('Location: http://www.google.com', true); // off to google!
        exit;
    }

    // user authenticated! display logs

    $files = scandir("logs/");
    sort($files, SORT_ASC);

    // print log files for viewing
?>

<html>

<head>
    <link rel="stylesheet" type="text/css" href="style.css">
    <meta http-equiv="refresh" content="15" >
    <title>Ethical Hacking Project</title>
</head>

<body>
    <center>
        <h1>Log Files</h1>
        <table >
            <tr>
                <th>Uploaded</th>
                <th>Filename</th>
            </tr>

            <?php
                $app_path = explode('/', $_SERVER['SCRIPT_NAME'])[1];
                // printing files
                foreach ($files as $file)
                {
                    if ($file == "." || $file == "..")
                        continue;

                    // grab the file log date from file name
                    $log_date = date_format(date_create_from_format('YmdHis', (explode('_', $file)[1])), 'm/d/Y h:i:s A');
                    $file_loc = "logs/$file";
                    echo "<tr><td><b>$log_date</b></td><td><a href=$file_loc>$file</a></td></tr>\n";
                }
            ?>
        </table>
        <br>
        <br>
        <i>This page will auto-reload every 15 seconds.</i>
    </center>
</body>

</html>
