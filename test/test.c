#include "greatest.h"

SUITE(FixedBufSuite);
SUITE(FlexBufSuite);
SUITE(LexerSuite);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(FixedBufSuite);
    RUN_SUITE(FlexBufSuite);
    RUN_SUITE(LexerSuite);

    GREATEST_MAIN_END();
}