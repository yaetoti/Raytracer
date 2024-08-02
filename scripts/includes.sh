cd $1
find . -type f \( -name "*.h" -o -name "*.hpp" \) | sed 's|^\./||' | sed 's|^|#include "|; s|$|"|'