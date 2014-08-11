unsigned char string[129]={"                "					//num play 0-15
						"                "					//num play 16-29
						"Start Game?     "					//init msg
						"RB0 = Start     "					//init msg
						"WRONG,try again?"					//wrong msg
						"RB0->y || RA5->N"					//wrong msg
						"Congratulations!"					//win msg
						"YOU WIN!        "};				//win msg



#define SWITCH_0 PORTBbits.RB0 
#define SWITCH_1 PORTAbits.RA5 
