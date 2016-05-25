#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "record.h"
#include "simtime.h"
#include <math.h>

#define ARRIVAL    1
#define DEPARTURE  2
#define CHANGE 3
#define ADAPT 4

#define H 1
#define L 0
#define LAMBDA_H 50
#define C_H 100

#define LAMBDA_L 9
#define C_L 10

#define P_H 10
#define P_L 1

#define SWITCH_LH 0.1
#define SWITCH_HL 0.1
#define H_time 60
#define L_time 60
#define QUEUE_LENGHT 30
//long seme = 14123451;
long seme = 141224725;

Event *event_list = NULL;
Record *queue=NULL;
Record *in_service=NULL;
double l[2]={LAMBDA_L,LAMBDA_H};
double m[2]={C_L, C_H};
double P[2]={P_L, P_H};
double ro_H=(double)LAMBDA_H/C_H;
double ro_L= (double)LAMBDA_L/C_L;
double p_loss_H;
double p_loss_L;

double lambda,mu,power;
int total_users;
int arrived_users;
int dropped_users;
double cumulative_time_user;
double cumulative_power;
Time total_delay,last_event_time;
double status_duration;
int number_of_samples;
Time current_time;

int status=0;
float switch_time[2]={SWITCH_LH, SWITCH_HL};

extern double negexp(double,long *);

void schedule(int type, Time time)
{
   Event *ev;
   ev = new_event();
   ev->type = type;
   ev->time = time;
   if (type==ADAPT)
   {
       ev->param1=status;
   }
   insert_event(&event_list,ev);
   return;
}

/*
**  Function : void get_input(char *format,void *variable)
**  Return   : None
**  Remarks  : To be used instead of scanf. 'format' is a scanf format,
**             'variable' the pointer to the variable in which the input
**             is written.
*/
void get_input(char *format,void *variable)
{
    static char linebuffer[255];
    char *pin;

    fgets(linebuffer, 255, stdin);	/*  Gets a data from stdin without  */
    pin = strrchr (linebuffer, '\n');	/*  flushing problems		    */
    if (pin!=NULL) *pin = '\0';

    sscanf(linebuffer,format,variable);	/* Read only the beginning of the   */
					/* line, letting the user to write  */
					/* all the garbage he prefer on the */
					/* input line.			    */
}

void arrival(void)
{ Time delta;
  Record *rec;
  delta = negexp(1.0/lambda,&seme);

  schedule(ARRIVAL,current_time+delta);
  rec = new_record();
  rec->arrival = current_time;
  cumulative_time_user+=total_users*(current_time-last_event_time);
  cumulative_power+=power*(current_time-last_event_time);
  arrived_users++;
  if(total_users<QUEUE_LENGHT)
  {
  total_users++;
  if (in_service == NULL)
    {
      in_service = rec;
      schedule(DEPARTURE,current_time+negexp(1/mu,&seme));
    }
  else
    in_list(&queue,rec);
  }
  else
  {
      dropped_users++;
  }
  return;

}

void departure(void)
{
  Record *rec;
  rec = in_service;
  in_service = NULL;
  cumulative_time_user+=total_users*(current_time-last_event_time);
  cumulative_power+=power*(current_time-last_event_time);
  total_users--;
  number_of_samples++;
  total_delay+=current_time - rec->arrival;
  release_record(rec);
  if (queue!=NULL)
   {
    in_service = out_list(&queue);
    schedule(DEPARTURE,current_time+negexp(1.0/mu,&seme));
   }
  return;
}
void change_status()
{
    if (status==1)
    {
        status=0;
        lambda=l[status];
        status_duration=negexp(H_time,&seme);
        schedule(CHANGE,current_time+status_duration);
        schedule(ADAPT,current_time+switch_time[!status]);
    }
    else{
        status=1;
        lambda=l[status];
        status_duration=negexp(L_time,&seme);
        schedule(CHANGE,current_time+status_duration);
        schedule(ADAPT,current_time+switch_time[!status]);
    }
    printf("Stato %d\t status duration %f\n",status,status_duration);
}
void adapt(int stat)
{
    if (stat==status)
    {
    mu=m[stat];
    power=P[stat];
    //printf("Adapdted to status %d\n",stat);
    }
    return;
}
double avg_dly(double x, double y)
{
    int i=0;
    double ro=x/y;
    double p0=(1-ro)/(1-pow(ro,QUEUE_LENGHT+1));
    double avg_n=0.0;
    double avg_l=0.0;
    for (i=1;i<=QUEUE_LENGHT;i++)
    {
        avg_n+=i*p0*pow(ro,i);
        avg_l+=x*p0*pow(ro,i-1);

    }
    return avg_n/avg_l;

}




void results(void)
{
 cumulative_time_user+=total_users*(current_time-last_event_time);
 printf("Final time %f\n",current_time);
 printf("Number of services %d\n",number_of_samples);
 printf("Number of arrival %d\n",arrived_users);
 printf("Average delay   %f\n",total_delay/number_of_samples);
 printf("delay H %f \t delay L %f\n",avg_dly(LAMBDA_H,C_H),avg_dly(LAMBDA_L,C_L));
 printf("\nAverage number of users %f\n",cumulative_time_user/current_time);
 printf("Probability of user drop %f \n",(float)(dropped_users)/(float)arrived_users);
 printf("P(l_h) %f\t p(l_l) %f\n",p_loss_H,p_loss_L);
 printf("Number of dropped users %d\n",dropped_users);
 printf("\nConsumed energy %f\n",cumulative_power);
 printf("Energy_max %f \t energy_min %f\n",current_time*P_H,current_time*P_L);
 printf("Saved energy %f -----> %f %%",current_time*P_H-cumulative_power,100*(current_time*P_H-cumulative_power)/(current_time*P_H));
 exit(0);
}

/*funzione che cambia stato del sistema rate alto o basso restituisce 1 se cambia stato*/
/*cambio di stato deve essere esponenziale*/
int main()
{
 Event *ev;
 Time maximum;

 cumulative_time_user =0.0;
 cumulative_power=0.0;
 total_delay=0.0;
 last_event_time=0.0;
 number_of_samples=0.0;

 current_time = 0.0;
 arrived_users = 0;
 total_users = 0;
 p_loss_H=(1-ro_H)*pow(ro_H,QUEUE_LENGHT)/(1-pow(ro_H,QUEUE_LENGHT+1));
 p_loss_L=(1-ro_L)*pow(ro_L,QUEUE_LENGHT)/(1-pow(ro_L,QUEUE_LENGHT+1));

 printf("Insert simulation time: ");
 get_input("%lf",&maximum);
 printf("Max Time  = %f\n",maximum);
/*fare versione schedulata uno scheduling di change tira l'altro*/
 //change_status();
 lambda=l[status];
 mu=m[status];
 power=P[status];
 schedule(ARRIVAL,negexp(1.0/lambda,&seme));
 status_duration=negexp(L_time,&seme);
 schedule(CHANGE,current_time+status_duration);

 while (current_time<maximum)
  {
    ev = get_event(&event_list);
    last_event_time = current_time;
    current_time = ev->time;
    switch (ev->type)
     {
       case ARRIVAL:   arrival();
		      break;
       case DEPARTURE: departure();
		      break;
       case CHANGE: change_status();
              break;
       case ADAPT: adapt(ev->param1);
              break;
       default:       printf("This is awful!!!\n");
		      exit(1);
     }
    release_event(ev);
  }
 results();
}


/* The real probability of dropping an user is always less then the worse probability calculated for each state*/
