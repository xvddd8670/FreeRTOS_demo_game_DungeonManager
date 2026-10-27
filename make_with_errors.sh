#/bin/bash

echo "------------" && \
make 2>&1 | grep -nE "undefined reference|multiple definition|overflowed|cannot find|No rule" | \
sed -E 's|/home/xv/code_projects/stm32/test_project/||g; s|/home/xv/.local/programms/arm-gnu-toolchain-15.3[^:]*ld: ||g' | \
awk '{print $0; print "------------"}'
