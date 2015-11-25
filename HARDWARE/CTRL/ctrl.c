#include "ctrl.h"
#include "pid.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "pwm.h"
#include "mymath.h"
#include "usart.h"
#include "kal.h"

float pitch,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
extern KAL now_omegax;
extern KAL init_omegax;
extern KAL init_roll;
extern KAL now_roll;
void ctrl1(ITEM exp_point)
{
	double k=0.4;
	
	PID1 pid_pitch;
	PID1 pid_roll;
	PID1 pid_yaw;
	
	PID1 pid_omegax;
	PID1 pid_omegay;
	PID1 pid_omegaz;
	
	double pid_pitch_out=0;
	double pid_roll_out=0;
	double pid_yaw_out=0;
	double pid_omegax_in=0;
  double pid_omegay_in=0;
  double pid_omegaz_in=0;	
	double pid_omegax_out=0;
	double pid_omegay_out=0;
	double pid_omegaz_out=0;
	double omegax=0;
	double omegay=0;
	double omegaz=0;
	OUTPUT all_out;
	OUTPUT all_out_f;
	
	pid_pitch=pid_set1(0,0,0);
	pid_roll=pid_set1(0,0,0);
	pid_yaw=pid_set1(0,0,0);
	
	pid_omegax=pid_set1(2.2,0.026,0);
	pid_omegay=pid_set1(6,0,0);
	pid_omegaz=pid_set1(1.1,2.2,3.3);
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{ 
//		temp=MPU_Get_Temperature();	//�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
	}
	omegax=((double)gyrox+26.00)/16.4;
	omegay=((double)gyroy+26.00)/16.4;
	omegaz=((double)gyroz+26.00)/16.4;
	
	now_omegax=kal(init_omegax,omegax);
	init_omegax=now_omegax;
	now_roll=kal(init_roll,roll);
	init_roll=now_roll;
	
	printf("%f\r\n",now_omegax.data);
//  printf("%f\r\n",omegay);
//	printf("%f\r\n",omegaz);
	printf("aaa\r\n");
	
	if((pitch<20)&&(pitch>-20)&&(roll<20)&&(roll>-20))
	{
    if((roll<1.5)&&(roll>-1.5))		
		{
//		all_out_f.back_thr=750;
//		all_out_f.head_thr=750;
		all_out_f.left_thr=750;
		all_out_f.right_thr=750;
		}
    else
    {			
			
	pid_pitch_out=pid_cal1(pid_pitch,pitch,exp_point.pitch);
	pid_roll_out=pid_cal1(pid_roll,now_roll.data,exp_point.roll);
	pid_yaw_out=pid_cal1(pid_yaw,yaw,exp_point.yaw);
	
	pid_omegax_in=now_omegax.data-k*pid_roll_out;
	pid_omegay_in=omegay-k*pid_pitch_out;
	pid_omegaz_in=omegaz-k*pid_yaw_out;
	
	pid_omegax_out=pid_cal1(pid_omegax,pid_omegax_in,0);
	pid_omegay_out=pid_cal1(pid_omegay,pid_omegay_in,0);
	pid_omegaz_out=pid_cal1(pid_omegaz,pid_omegaz_in,0);
	

		//	pid_omegax_out=0;
	pid_omegay_out=0;
	pid_omegaz_out=0;
	
//	all_out.back_thr=exp_point.thr+pid_omegax_out-pid_omegay_out+pid_omegaz_out;
//	all_out.head_thr=exp_point.thr+pid_omegax_out+pid_omegay_out+pid_omegaz_out;
	all_out.left_thr=exp_point.thr+pid_omegax_out-pid_omegay_out-pid_omegaz_out;
	all_out.right_thr=exp_point.thr-pid_omegax_out+pid_omegay_out-pid_omegaz_out;
	 
//	  all_out_f.back_thr=limit(all_out.back_thr,1000,500);
//	  all_out_f.head_thr=limit(all_out.head_thr,1000,500);
	  all_out_f.left_thr=limit(all_out.left_thr,1000,500);
		all_out_f.right_thr=limit(all_out.right_thr,1000,500);
	//	 printf("%d\r\n",all_out_f.right_thr);
    }
	}
	else
	{
//		all_out_f.back_thr=500;
//		all_out_f.head_thr=500;
		all_out_f.left_thr=500;
		all_out_f.right_thr=500;		
	}
	pwm_out(all_out_f);
}

ITEM set_point(double pitch,double roll,double yaw,u16 thr)
{
	ITEM point;
	point.pitch=pitch;
	point.roll=roll;
	point.yaw=yaw;
	point.thr=thr;
	
	return point;
}
