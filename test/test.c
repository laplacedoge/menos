#include "greatest.h"

SUITE(FixedBufSuite);
SUITE(FlexBufSuite);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(FixedBufSuite);
    RUN_SUITE(FlexBufSuite);

    GREATEST_MAIN_END();
}