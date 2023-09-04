#include "./util/main.h"

SwitchController controller1(32, 8);
SwitchController controller1(32, 13);

void setup() {}

void loop() { controller1.run(); }