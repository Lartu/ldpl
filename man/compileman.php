<?php
    //Load all pages from the gitbook
    $summary = file_get_contents("manList");
    $summary = explode("\n", $summary);
    $manPage = ".ce 1\n.SH -=-=-=-=-=-=-=-=-=- DOCS: LDPL DOCUMENTATION -=-=-=-=-=-=-=-=-=-\n";
    foreach($summary as $line){
        $line = trim($line);
        if(strlen($line) > 0){
            $manPage = $manPage . "\n\n" . file_get_contents($line);
        }
        else if(strlen($line) > 0 && trim(substr($line, 0, 2)) == "##"){
            $manPage = $manPage . "\n\n" . ".ce 1\n.SH -=-=-=-=- DOCS: " . trim(strtoupper(substr($line, 2))) . " -=-=-=-=-";
        }
    }

    //Replace all originally blank lines with <BLANK>
    $manPage = explode("\n", $manPage);
    foreach($manPage as &$line){
        if(strlen(trim($line)) == 0){
            $line = "<BLANK>";
        }
    }
    $manPage = implode("\n", $manPage);

    $manPage = str_replace("\(", "(", $manPage);
    $manPage = str_replace("\)", ")", $manPage);
    $manPage = str_replace('{% hint style="info" %}', "-- Note:\n.br", $manPage);
    $manPage = str_replace('{% hint style="warning" %}', "-- Warning:\n.br", $manPage);
    $manPage = str_replace('{% endhint %}',      ".br\n--", $manPage);
    $manPage = str_replace('{% endcode-tabs-item %}', "", $manPage);
    $manPage = str_replace('{% endcode-tabs %}', "", $manPage);
    $manPage = str_replace('{% code-tabs %}', "", $manPage);
    $manPage = str_replace('{% code-tabs-item title="', "File: ", $manPage);
    $manPage = str_replace('" %}', "\n.br", $manPage);
    $manPage = str_replace("\\", "\\\\", $manPage);
    $manPage = str_replace("**", "", $manPage);
    $manPage = str_replace("_ ", " ", $manPage);
    $manPage = str_replace(" _", " ", $manPage);
    $manPage = str_replace("\[", "[", $manPage);
    $manPage = str_replace("\]", "]", $manPage);
    $manPage = str_replace("\#", "#", $manPage);
    $manPage = str_replace("&gt;", ">", $manPage);
    $manPage = str_replace("&lt;", "<", $manPage);
    $manPage = preg_replace("/\[(.*?)\]\(.*?\)/m", "$1", $manPage);

    $manPage = explode("\n", $manPage);

    $incode = false;
    $lineNum = 1;

    //Convert each page to groff
    foreach($manPage as &$line){

        $line = rtrim($line);

        if(strlen($line) > 0 && substr($line, 0, 3) == "```"){
            $incode = !$incode;
            if($incode) $lineNum = 1;
            $line = "";
        }
        else if($incode){
            if($lineNum < 10)
                $line = $lineNum . " | " . $line . "\n.br";
            else
            $line = $lineNum . "| " . $line . "\n.br";
            $lineNum++;
        }
        else if(strlen($line) > 0 && trim(substr($line, 0, 4)) == "####"){
            $line = ".B " . trim(substr($line, 4));
        }
        if(strlen($line) > 0 && trim(substr($line, 0, 3)) == "###"){
            $line = ".B " . trim(substr($line, 3));
        }
        else if(strlen($line) > 0 && trim(substr($line, 0, 2)) == "##"){
            $line = ".B " . trim(substr($line, 2));
        }
        else if(strlen($line) > 0 && substr($line, 0, 1) == "#"){
            $line = ".SH " . strtoupper(trim(substr($line, 1)));
        }
        else if(strlen($line) > 0 && substr($line, 0, 1) == "|"){
            $line = ".br\n" . trim($line);
        }
        else if(strlen($line) > 0 && substr($line, 0, 2) == "* "){
            $line = ".br\n[*] " . trim(substr($line, 2));
        }
        else if($line == "!"){
            $line = "";
        }

        $line = trim($line);
    }

    $manPage = implode("\n", $manPage);

    $manPage = str_replace('`', "'", $manPage);
    $manPage = str_replace("\n\n", "\n", $manPage, $count);
    $manPage = str_replace("\n<BLANK>\n", "\n\n", $manPage, $count);
    $manPage = str_replace("<BLANK>", "", $manPage, $count);
    echo file_get_contents("ldplman-intro") . "\n" . $manPage . "\n";
?>
