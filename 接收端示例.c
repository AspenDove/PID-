		//这些都写在串口中断里
		static uint16_t speed,iKp, iTi, tTi1, tTi2, iTd;
		
		speed = uart6_rx[0];
		speed <<= 8;
		speed += uart6_rx[1];
		setspeed[0] = (float)speed/10000.f;
		iKp = uart6_rx[2];
		iKp <<= 8;
		iKp += uart6_rx[3];
		iTi = uart6_rx[4];
		iTi <<= 8;
		iTi += uart6_rx[5];
		iTd = uart6_rx[6];
		iTd <<= 8;
		iTd += uart6_rx[7];
		
		Adjust(&atti[0][0], (float)iKp / 1.e5f, (float)iTi / 1.e4f, (float)iTd / 1.e3f);
		Adjust(&atti[1][0], (float)iKp / 1.e5f, (float)iTi / 1.e4f, (float)iTd / 1.e3f);
	
		iKp = uart6_rx[8];
		iKp <<= 8;
		iKp += uart6_rx[9];
		iTi = uart6_rx[10];
		iTi <<= 8;
		iTi += uart6_rx[11];	
		if (tTi2 != iTi)
			pid[11].error[INTEGRATE] = 0;
		tTi2 = iTi;
		iTd = uart6_rx[12];
		iTd <<= 8;
		iTd += uart6_rx[13];
		
		Adjust(&atti[0][1], (float)iKp / 1.e4f, (float)iTi / 1.e5f, (float)iTd / 1.e3f);
		Adjust(&atti[1][1], (float)iKp / 1.e4f, (float)iTi / 1.e5f, (float)iTd / 1.e3f);
