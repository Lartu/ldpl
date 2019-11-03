#!/usr/bin/env bash
cp -r ../docs docs
#git clone https://www.github.com/lartu/ldpl-docs
cp compileman.php docs/
cp ldplman-intro docs/
cd docs
php compileman.php > ldpl.1
cp ldpl.1 ..
cd ..
rm -rf docs
