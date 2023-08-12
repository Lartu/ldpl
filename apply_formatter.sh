find . -regex '.*\.\(cpp\|h\)' -exec clang-format -Werror -style=Google -i {} \;
