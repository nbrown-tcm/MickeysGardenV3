#include <Adafruit_NeoPixel.h>
//#include <Ramp.h>                           


// Pattern types supported:
enum  pattern { NONE_, TRICKLE, SPLOOSH, SCANNER, SPLASH };
// Patern directions supported:
enum  direction { FORWARD_, REVERSE_ };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:

    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    bool lightShouldBeMoving = true;
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern

    float pudSpeed = 1.7;
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
          //myRamp.go(10 , pixels * Interval, LINEAR, LOOPBACKWARD);    

    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case NONE_:
                    NoneUpdate();
                    break;
                case TRICKLE:
                    TrickleUpdate();
                    break;
                case SPLOOSH:
                    SplooshUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case SPLASH:
                    SplashUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD_)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }
    
    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD_)
        {
            Direction = REVERSE_;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD_;
            Index = 0;
        }
    }

    void None()
    {

        ActivePattern = NONE_;
        Interval = 0;
        TotalSteps = 0;
        Index = 0;

    }

    void NoneUpdate()
    {

        Color(0, 0, 0);
        clear();
        show();
      
    }

    void Sploosh(uint8_t interval, direction dir = FORWARD_)
    {

        ActivePattern = SPLOOSH;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;

    }

    void SplooshUpdate()
    {
    }
    
    void Trickle(uint32_t color1, uint8_t interval, direction dir = REVERSE_)
    {

        ActivePattern = TRICKLE;
        Interval = interval;
        //TotalSteps = (numPixels() - 1) * 2;
        TotalSteps = 38;
        //Direction = dir;
        Color1 = color1;
        Index = 1;

    }

    void TrickleUpdate()
    {


                 Serial.println("Trickling");


        for (int i = 0; i < 38; i++)
        {
             if (i == TotalSteps - Index) // Scan Pixel to the left
             {
                 setPixelColor(i, Color1);

             }
             //else if (i == TotalSteps) ActivePattern = NONE_;

             else // Fading tail
             {
                  setPixelColor(i, DimColor(getPixelColor(i)));
             }
        }
        show();
        Increment();




    }

  void Splash(int interval)
  {

        ActivePattern = SPLASH;
        Interval = interval;
        //TotalSteps = (numPixels() - 1) * 2;
        TotalSteps = 38;
        //Direction = dir;
        //Color1 = color1;
        Index = 1;

  }


  void SplashUpdate()
  {


    //used golden ratio to calculate this

    // unsigned long currentMillis = millis();

    //  if(currentMillis - lastUpdate >= Interval)
    //  {
    RingLightUp(0, 8, pudSpeed * 50);
    RingLightUp(8, 16, pudSpeed * 80.9);
    RingLightUp(24, 24, pudSpeed * 130.8962);
    RingLightUp(48, 35, pudSpeed * 211.7900516);
    RingLightUp(83, 45, pudSpeed * 342.676303489);
    //lastUpdate = currentMillis;

    Serial.println("SPlash!!");

    ActivePattern = NONE_;

    //}

    clear();

  }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval, direction dir = REVERSE_)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 30;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
      


        if((millis() - lastUpdate) > Interval) // time to update
        {

        for (int i=50;  i--> 0;)
        {
            if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
                  Serial.println("Scanning the other way now");
                  Serial.println(i);

            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        
        }
        show();
        Increment();
        }
    }

    void RingLightUp(int start, int length, int del)
    {

    fill((25, 25, 200), start, length);
    show();
    delay(del);
    clear();
    Serial.println("Lighting...");

  }
  
   
    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
    
};


void PuddleComplete();
void Ring1Complete();
void Ring2Complete();
void Ring3Complete();

NeoPatterns PuddleRing(128, 8, NEO_GRB + NEO_KHZ800, &PuddleComplete);
NeoPatterns Ring1(40, 9, NEO_GRB + NEO_KHZ800, &Ring1Complete);
NeoPatterns Ring2(40, 10, NEO_GRB + NEO_KHZ800, &Ring2Complete);
NeoPatterns Ring3(40, 11, NEO_GRB + NEO_KHZ800, &Ring3Complete);

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit VS1053 Simple Test");

  //PuddleRing.clear();

  PuddleRing.ActivePattern = NONE_;
  //PuddleRing.Splash(5);
  //Ring1.Trickle(Ring1.Color(25,25,200), 1);
  //Ring2.Trickle(Ring2.Color(25,25,200), 1);
  //Ring3.Trickle(Ring3.Color(25,25,200), 1);

  Ring1.ActivePattern = NONE_;
  Ring2.ActivePattern = NONE_;
  Ring3.ActivePattern = NONE_;


  //Ring1.Scanner(Ring1.Color(255,0,0), 55);

  PuddleRing.begin();
  Ring1.begin();
  Ring2.begin();
  Ring3.begin();


}


void loop() {

    int val = digitalRead(5);

    if (val == 0) //Beam is broken
    {
      //Serial.println("Water the damn plant already");
    }
    else
    {
      PuddleRing.ActivePattern = SPLASH;

      PuddleRing.Splash(5); 
      Ring1.ActivePattern = TRICKLE;
      Ring1.Trickle(Ring1.Color(25,25,200), 1);
      Ring2.ActivePattern = TRICKLE;
      Ring2.Trickle(Ring2.Color(25,25,200), 1);
      Ring3.ActivePattern = TRICKLE;
      Ring3.Trickle(Ring3.Color(25,25,200), 1);  
    }


  if (Serial.available()) {
    char c = Serial.read();
    
    if (c == 'w') {

  PuddleRing.ActivePattern = SPLASH;

  PuddleRing.Splash(5); 
  Ring1.ActivePattern = TRICKLE;
  Ring1.Trickle(Ring1.Color(25,25,200), 1);
  Ring2.ActivePattern = TRICKLE;
  Ring2.Trickle(Ring2.Color(25,25,200), 1);
  Ring3.ActivePattern = TRICKLE;
  Ring3.Trickle(Ring3.Color(25,25,200), 1);  




Serial.println("Sploosh");

      }
  }


  delay(100);

  PuddleRing.Update();
  Ring1.Update();
  Ring2.Update();
  Ring3.Update();


}



void PuddleComplete()
{
    //PuddleRing.clear();
    //PuddleRing.None();

    //PuddleRing.lightShouldBeMoving = false;

}

void Ring1Complete()
{

  //Ring1.None();
  //Ring1.clear();
  //Ring1.lightShouldBeMoving = false;
  Serial.println("Flow done");
  Ring1.ActivePattern = NONE_;


}

void Ring2Complete()
{

  //Ring2.None();
  //Ring2.clear();
  //Ring1.lightShouldBeMoving = false;
  Serial.println("Flow done");
  Ring2.ActivePattern = NONE_;


}

void Ring3Complete()
{

  //Ring3.None();
  //Ring3.clear();
  //Ring1.lightShouldBeMoving = false;
  Serial.println("Flow done");
  Ring3.ActivePattern = NONE_;


}

