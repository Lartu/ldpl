#!/usr/bin/env bash
git clone https://www.github.com/lartu/ldpl-docs
cp compileman.php ldpl-docs/
cp ldplman-intro ldpl-docs/
cd ldpl-docs
php compileman.php > ldpl.1
cp ldpl.1 ..
cd ..
rm -rf ldpl-docs