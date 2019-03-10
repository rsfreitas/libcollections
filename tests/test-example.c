
/*
 * Description: Just an example of using cutter as Unit Testing for axmobility
 *              exported API and internal functions.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Feb 12 13:53:27 -02 2019
 */

#include <cutter.h>

void test_condition(void);

static int condition = 0;

void cut_setup(void)
{
    condition = 1;
}

void cut_teardown(void)
{
    condition = 0;
}

void test_condition(void)
{
    cut_set_message("The condition value should be set to 1 in cut_setup()");
    cut_assert_equal_int(1, condition);
}

