# ScissorLiftStage
Calibration for stepper motor driven scissor lift stage.

Scissor lift stages are not linear stages. Many are driven by a stepper motor. The stepper motor will move the bottom of the scissor stage a fixed amount per revolution (likely 1 mm/1 revolution), but this does not translate to 1mm lift, or even a constant rate of lift.

The motion can be analyzed using simple geometry. The results are shown in the jpg image.


Later we used a Huanyu stage from Amazon.com, measured the proper parameters, and used a nonlinear optimization to find the effective stage parameters to calibrate the stage. We then were able to back-calculate the number of required revolutions of the stepper motor to achieve an arbitrary vertical movement. This is shown in the .ipynb

We then interfaced an Arduino Uno with the stepper motor control (a standard one, please don't use the Arduino stepper motor control, it did not work for us) to drive the motor and monitor the limit switches simultaneously. An example of a Z stage class to operate the motor is given.
