<?php
    //This file converts ldpl_lib.h to ldpl_included_lib.h

    $lib = file_get_contents("ldpl_lib.cpp");
    $lib = str_replace('"', '\\"', $lib);
    $lib = str_replace("\\n", "\\\\n", $lib);
    $lines = explode("\n", $lib);
    echo "void add_ldpllib(compiler_state & state){\n";
    foreach($lines as $line){
        echo 'state.add_var_code("' . trim($line) . '");' . "\n";
    }
    echo "}";
?>
