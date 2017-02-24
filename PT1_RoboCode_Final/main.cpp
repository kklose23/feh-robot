#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

FEHMotor lWheel (FEHMotor:: Motor0, 12.0);
FEHMotor rWheel (FEHMotor:: Motor1, 12.0);
AnalogInputPin cdsCell (FEHIO:: P1_0);
DigitalEncoder lEncode (FEHIO:: P0_0);
DigitalEncoder rEncode (FEHIO:: P0_1);

#define START_LIGHT 1.5
#define AXLE_LENGTH 7.86
#define PI 3.14159
#define WHEEL_R 1.5

int calcTicks(float inches){
    const int oneRevTicks = 318;
    float circ = 2 * PI * WHEEL_R;
    return (inches*oneRevTicks)/circ;
}

void driveStraight(float dist, int power){
    int ticks = calcTicks(dist);
    lEncode.ResetCounts();
    rEncode.ResetCounts();

    lWheel.SetPercent(power);
    rWheel.SetPercent(power);
    while((lEncode.Counts() + rEncode.Counts()) / 2 < ticks);

    lWheel.Stop();
    rWheel.Stop();

}

void turn (char dir, int degrees, int power){
    float inches = (degrees/360.0) * (2*PI*(AXLE_LENGTH/2.0));

    int ticks = calcTicks(inches);
    lEncode.ResetCounts();
    rEncode.ResetCounts();

    if(dir == 'r'){
        rWheel.SetPercent(-1*power);
        lWheel.SetPercent(power);
    }else{
        rWheel.SetPercent(power);
        lWheel.SetPercent(-1*power);
    }

    while((lEncode.Counts() + rEncode.Counts()) / 2 < ticks);
    lWheel.Stop();
    rWheel.Stop();
}

void accelerate(int power, float length){

}

void holdButton(int power,float time){
    rWheel.SetPercent(power);
    lWheel.SetPercent(power);
    lEncode.ResetCounts();
    rEncode.ResetCounts();
    int leftCount = lEncode.Counts();
    int rightCount = rEncode.Counts();
    float start = TimeNow();
    do{
        LCD.Write("Enter sleep");
        leftCount = lEncode.Counts();
        rightCount = rEncode.Counts();
        Sleep(1.0);
        LCD.WriteLine(" Done sleeping");
        LCD.WriteLine("Looking for the button");
    }while((leftCount + rightCount + 7) <= (lEncode.Counts() + rEncode.Counts()) && (TimeNow() - start) < 5);
    LCD.WriteLine("Holding Button");
    Sleep(time);
}

void waitForCds(){
    float start = TimeNow();
    while(cdsCell.Value() > START_LIGHT || TimeNow() - start > 30);
}


int main(void){
    LCD.Clear( FEHLCD::Black);
    LCD.SetFontColor( FEHLCD::White);

    waitForCds();
    driveStraight(11.0,20);
    turn('l',90, 20);
    driveStraight(12.0,30);
    turn('r',90, 20);
    driveStraight(28.0,-33);
    turn('l',90, 20);
    driveStraight(7.5,30);
    turn('r',90, 20);
    driveStraight(16,-30);
    holdButton(-20,5.5);
    driveStraight(19.5,30);
    turn('r',90, 20);
    driveStraight(18.0,25);
    return 0;
}
