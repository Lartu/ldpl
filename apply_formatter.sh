find src -regex '.*\.\(cpp\|h\)' -exec clang-format -Werror -style=Google -i {} \;
