
#include <stdio.h>

struct test_ptr {
    int x;
    int y;
    int z;
};

void rust_call(struct test_ptr *ptr)
{
    printf("%s: %p\n", __FUNCTION__, ptr);
    printf("%s: x=%d, y=%d, z=%d\n", __FUNCTION__,
            ptr->x, ptr->y, ptr->z);

    ptr->x += 1;
}

