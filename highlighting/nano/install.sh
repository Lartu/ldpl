#!/usr/bin/env bash

# Check if ~/.nano directory exists
if [ ! -d "$HOME/.nano" ]; then
    echo "Creating ~/.nano directory..."
    mkdir ~/.nano
fi

# Copy ldpl.nanorc to ~/.nano
echo "Copying ldpl.nanorc to ~/.nano"
cp ldpl.nanorc ~/.nano/ldpl.nanorc

# Add ldpl.nanorc to .nanorc
echo "include $HOME/.nano/ldpl.nanorc" >> ~/.nanorc
