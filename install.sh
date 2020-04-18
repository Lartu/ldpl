#!/usr/bin/env bash

BACKTITLE="LDPL Install Wizard (www.ldpl-lang.org)"
TITLE="LDPL Installation"

whiptail --backtitle "$BACKTITLE" --msgbox "Welcome to the LDPL install wizard. This script will build and install LDPL for you." 10 50 --title "$TITLE"

if ! [ -x "$(command -v c++)" ]; then
    whiptail --backtitle "$BACKTITLE" --msgbox "You need to have a C++ compiler installed and aliased to 'c++' on your PATH to continue." 10 50 --title "Error"

    answer=$(whiptail --title "$TITLE" --backtitle "$BACKTITLE" --yesno "Would you like me to try to install a C++ compiler (g++) for you?" 10 50 3>&1 1>&2 2>&3; echo $?)

    if [ "$answer" = "0" ]; then
        if [ -f "/etc/redhat-release" ]; then
            sudo yum install g++
        elif [ -f "/etc/arch-release" ]; then
            sudo pacman -S g++
        elif [ -f "/etc/gentoo-release" ]; then
            whiptail --backtitle "$BACKTITLE" --msgbox "Sorry, emerge is not supported yet. Please install a C++ compiler by yourself and try again." 10 50 --title "Error"
            exit 1
        elif [ -f "/etc/SuSE-release" ]; then
            sudo zypper install g++
        elif [ -f "/etc/debian_version" ]; then
            sudo apt-get install g++
        else
            whiptail --backtitle "$BACKTITLE" --msgbox "Sorry, we couldn't install C++ for you. Please install a C++ compiler by yourself and try again." 10 50 --title "Error"
            exit 1
        fi

        if ! [ -x "$(command -v c++)" ]; then
            whiptail --backtitle "$BACKTITLE" --msgbox "Automatic installation failed. Please install a C++ compiler by yourself and try again." 10 50 --title "Error"
        else
            whiptail --backtitle "$BACKTITLE" --msgbox "The 'g++' C++ compiler has been installed." 10 50 --title "$TITLE"
        fi
    else
        whiptail --backtitle "$BACKTITLE" --msgbox "Please install a C++ compiler by yourself and try again." 10 50 --title "Error"
        exit 1
    fi
fi

if ! [ -x "$(command -v make)" ]; then
    whiptail --backtitle "$BACKTITLE" --msgbox "You need to have a 'make' installed and on your PATH to continue." 10 50 --title "Error"

    answer=$(whiptail --title "$TITLE" --backtitle "$BACKTITLE" --yesno "Would you like me to try to install 'make' for you?" 10 50 3>&1 1>&2 2>&3; echo $?)

    if [ "$answer" = "0" ]; then
        if [ -f "/etc/redhat-release" ]; then
            sudo yum install make
        elif [ -f "/etc/arch-release" ]; then
            sudo pacman -S make
        elif [ -f "/etc/gentoo-release" ]; then
            whiptail --backtitle "$BACKTITLE" --msgbox "Sorry, emerge is not supported yet. Please install 'make' by yourself and try again." 10 50 --title "Error"
            exit 1
        elif [ -f "/etc/SuSE-release" ]; then
            sudo zypper install make
        elif [ -f "/etc/debian_version" ]; then
            sudo apt-get install make
        else
            whiptail --backtitle "$BACKTITLE" --msgbox "Sorry, we couldn't install 'make' for you. Please install 'make' and try again." 10 50 --title "Error"
            exit 1
        fi

        if ! [ -x "$(command -v make)" ]; then
            whiptail --backtitle "$BACKTITLE" --msgbox "Automatic installation failed. Please install 'make' by yourself and try again." 10 50 --title "Error"
        else
            whiptail --backtitle "$BACKTITLE" --msgbox "'make' has been installed." 10 50 --title "$TITLE"
        fi
    else
        whiptail --backtitle "$BACKTITLE" --msgbox "Please install a 'make' by yourself and try again." 10 50 --title "Error"
        exit 1
    fi
fi

cd src
make clean
make

whiptail --backtitle "$BACKTITLE" --msgbox "LDPL has been built. Please enter your password to enter sudo mode and install it." 10 60  --title "$TITLE"

if ! [ "$(sudo make install)" ]; then
    whiptail --backtitle "$BACKTITLE" --msgbox "LDPL has been built in the ./src directory, but has not been installed. You can install it by yourself or use it from there. Have a nice day!" 10 50 --title "$TITLE"
    echo "Rawrrr!"
    exit 1
fi

sudo make install && make clean

if ! [ -x "$(command -v ldpl)" ]; then
    whiptail --backtitle "$BACKTITLE" --msgbox "Installation failed. See the messages left by 'make' for more information." 10 50 --title "Error"
    exit 1
fi

whiptail --backtitle "$BACKTITLE" --msgbox "LDPL has been installed successfully. Run 'ldpl -h' for more information." 10 50 --title "$TITLE"
whiptail --backtitle "$BACKTITLE" --msgbox "Thank you for choosing LDPL!\nRead the docs at docs.ldpl-lang.org and visit us at www.ldpl-lang.org.\nHave a nice day!" 10 50 --title "$TITLE"

echo "Rawrrr!"
exit 0






