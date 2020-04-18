#!/usr/bin/env bash

clear
echo ""
echo "\e[92m--- LDPL Install Wizard (www.ldpl-lang.org) ---\e[0m"
echo ""
echo "\e[93m>\e[0m Welcome to the LDPL install wizard."
echo "  This script will build and install LDPL for you."
echo ""
printf "\e[91m?\e[0m "
read -p "Would you like to continue? (y/n) " REPLY
echo ""
if [ "$REPLY" = "y" ]; then
    # Install g++
    if ! [ -x "$(command -v c++)" ]; then
        echo "\e[93m>\e[0m You need to have a C++ compiler installed"
        echo "  and aliased to 'c++' on your PATH to continue."
        echo ""
        printf "\e[91m?\e[0m "
        read -p "  Would you like me to try to install a C++ compiler (g++) for you? (y/n) "
        echo ""
        if [ "$REPLY" = "y" ]; then
            if [ -f "/etc/redhat-release" ]; then
                sudo yum install g++
            elif [ -f "/etc/arch-release" ]; then
                sudo pacman -S g++
            elif [ -f "/etc/gentoo-release" ]; then
                echo "\e[93m>\e[0m Sorry, emerge is not supported yet."
                echo "  Please install a C++ compiler by yourself and try again."
                echo ""
                exit 1
            elif [ -f "/etc/SuSE-release" ]; then
                sudo zypper install g++
            elif [ -f "/etc/debian_version" ]; then
                sudo apt-get install g++
            else
                echo "\e[93m>\e[0m Sorry, we couldn't install C++ for you."
                echo "  Please install a C++ compiler by yourself and try again."
                echo ""
                exit 1
            fi
            if ! [ -x "$(command -v c++)" ]; then
                echo "\e[93m>\e[0m Automatic installation failed."
                echo "  Please install a C++ compiler by yourself and try again."
            else
                echo "\e[93m>\e[0m The GNU C++ compiler has been installed."
            fi
        else
            echo "\e[93m>\e[0m Please install a C++ compiler by yourself and try again."
            echo ""
            exit 1
        fi
    else
        echo "\e[93m>\e[0m C++ compiler found."
    fi

    echo ""

    # Install make
    if ! [ -x "$(command -v make)" ]; then
        echo "\e[93m>\e[0m You need to have a 'make' installed and on your PATH to continue."
        echo ""
        printf "\e[91m?\e[0m "
        read -p "Would you like me to try to install 'make' for you? (y/n) " REPLY
        echo ""
        if [ "$REPLY" = "y" ]; then
            if [ -f "/etc/redhat-release" ]; then
                sudo yum install make
            elif [ -f "/etc/arch-release" ]; then
                sudo pacman -S make
            elif [ -f "/etc/gentoo-release" ]; then
                echo "\e[93m>\e[0m Sorry, emerge is not supported yet."
                echo "  Please install 'make' by yourself and try again."
                echo ""
                exit 1
            elif [ -f "/etc/SuSE-release" ]; then
                sudo zypper install make
            elif [ -f "/etc/debian_version" ]; then
                sudo apt-get install make
            else
                echo "\e[93m>\e[0m Sorry, we couldn't install C++ for you."
                echo "  Please install 'make' by yourself and try again."
                echo ""
                exit 1
            fi

            if ! [ -x "$(command -v make)" ]; then
                echo "\e[93m>\e[0m Automatic installation failed."
                echo "  Please install 'make' by yourself and try again."
            else
                echo "\e[93m>\e[0m 'make' has been installed."
            fi
        else
            echo "\e[93m>\e[0m Please install a 'make' by yourself and try again."
            echo ""
            exit 1
        fi
    else
        echo "\e[93m>\e[0m Make found."
    fi

    echo ""

    # make
    echo "\e[93m>\e[0m Building LDPL..."
    echo ""
    cd src
    make clean
    make

    # Install
    echo ""
    echo "\e[93m>\e[0m LDPL has been built."
    echo ""
    printf "\e[91m?\e[0m "
    read -p "Would you like me to install it for you? (y/n) " REPLY
    echo ""
    if [ "$REPLY" = "y" ]; then
        echo "\e[91m\$\e[0m Please enter your password to enter sudo mode and install it."
        if ! [ "$(sudo make install)" ]; then
            echo ""
            echo "\e[93m>\e[0m LDPL has been built in the ./src directory, but has not been installed."
            echo "  You can install it by yourself or use it from there. Have a nice day!"
            echo "\e[93m>\e[0m Rawrrr!"
            echo ""
            exit 0
        fi
    else
        echo ""
        echo "\e[93m>\e[0m Ok! You will find the ldpl executable in the ./src directory. Have fun!"
        echo "\e[93m>\e[0m Rawrrr!"
        echo ""
        exit 0
    fi

    # Post-install
    echo ""
    sudo make install && make clean
    echo ""

    if ! [ -x "$(command -v ldpl)" ]; then
        echo "\e[93m>\e[0m Installation failed. See the messages left by 'make' for more information."
        echo ""
        exit 1
    fi

    echo "\e[93m>\e[0m LDPL has been installed successfully."
    echo "  Run 'ldpl -h' for more information."
    echo ""
    echo "\e[93m>\e[0m Thank you for choosing LDPL!"
    echo "  Read the docs at docs.ldpl-lang.org"
    echo "  and visit us at www.ldpl-lang.org."
    echo "  Have a nice day!"

    echo ""

    echo "\e[93m>\e[0m Rawrrr!"
    echo ""
    exit 0
else
    echo "\e[93m>\e[0m Rawrrr!"
    echo ""
    exit 0
fi