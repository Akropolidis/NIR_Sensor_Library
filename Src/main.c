#include "stm32f4xx.h"
#include "motor_driver.h"

#define MOTORDIAMETER	60 //Motor Diameter

int main(void)
{
//	uart2_rxtx_init();
//	tim2_pa5_pwm();
//	Tim2_Ch1_Init();
	Tim5_Ch2_Init();
//	set_Servo_pos90deg();
//	set_Servo_neg90deg();

	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(20);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(40);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(60);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(80);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(100);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(120);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(140);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(160);
	for (int i = 0; i < 1000000; i++){}
	set_servo_angle(180);

//	while(1)
//	{
//		for (int i = 0; i <= 18; i++)
//		{
//			for (int j = 0; j < 1000000; j++){}
//			set_servo_angle(i * 10);
//			// for (int i = 0; i < 1000000; i++){}
//		}
//		// int distance = Motor_B_Dist_mm(MOTORDIAMETER);
//		// printf("Distance: %d mm\n\n\r", distance);
//	 }
}
