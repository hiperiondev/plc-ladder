#include "config.h"
#include "hardware.h"
#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"

int project_task(plc_t p) { //
/**************start editing here***************************/
    uint8_t one, two, three;
    one = resolve(p, BOOL_DI, 1);
    two = fe(p, BOOL_DI, 2);
    three = re(p, BOOL_DI, 3);
    /*  contact(p,BOOL_DQ,1,one);
     contact(p,BOOL_DQ,2,two);
     contact(p,BOOL_DQ,3,three);      */
    if (one)
        set(p, BOOL_TIMER, 0);
    if (three)
        reset(p, BOOL_TIMER, 0);
    if (two)
        down_timer(p, 0);
    return 0;
    /***************end of editable portion***********************/

}
int project_init() {
    /*********************same here******************************/
    return 0;
}
