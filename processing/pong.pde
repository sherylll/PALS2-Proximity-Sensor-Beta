/**
 * This example works with the Arduino example pong for the PALS-2 sensor
 * It is modified from this sketch: https://www.openprocessing.org/sketch/47481
 */

import processing.serial.*;
import java.util.Scanner;

ComPortMenu portSelect;
String inString;

float x = 150;
float y = 150;
float speedX = random(3, 5);
float speedY = random(3, 5);
int leftColor = 128;
int rightColor = 128;
int diam;
int rectSize = 150;
float diamHit;

int distance;

void setup()
{
  size(640, 480);
  noStroke();
  smooth();
  ellipseMode(CENTER);
  textFont(createFont("Arial", 16, true));
  portSelect = new ComPortMenu();
}

void draw()
{
  background(255);
  fill(128, 128, 128);
  diam = 20;
  ellipse(x, y, diam, diam);

  portSelect.drawMenu();
  Serial comPort = portSelect.getComPort();
  if (comPort != null)
  {
    try
    {
      comPort.bufferUntil('\n');
      Scanner comInput = new Scanner(inString);
      comInput.useLocale(java.util.Locale.US);
      while (!comInput.hasNextInt())
        comInput.next();
      distance  = 480-(comInput.nextInt()-10)*15;
      comInput.close();
      visualize(distance);
    }
    catch(Exception e)
    {
      System.err.print("Invalid COM input: ");
      System.err.println(inString);
      System.err.print("Original message: ");
      System.err.println(e.getMessage());
    }
  }
}

void visualize(int distance)
{
  fill(leftColor);
  rect(0, 0, 20, height);
  fill(rightColor);
  rect(width-30, distance-rectSize/2, 10, rectSize);

  x = x + speedX;
  y = y + speedY;

  // if ball hits movable bar, invert X direction and apply effects
  if ( x > width-30 && x < width -20 && y > distance-rectSize/2 && y < distance+rectSize/2 ) {
    speedX = speedX * -1;
    x = x + speedX;
    rightColor = 0;
    fill(random(0, 128), random(0, 128), random(0, 128));
    diamHit = random(75, 150);
    ellipse(x, y, diamHit, diamHit);
    rectSize = rectSize-10;
    rectSize = constrain(rectSize, 10, 150);
  } 

  // if ball hits wall, change direction of X
  else if (x < 25) {
    speedX = speedX * -1.1;
    x = x + speedX;
    leftColor = 0;
  } else {     
    leftColor = 128;
    rightColor = 128;
  }
  // resets things if you lose
  if (x > width) { 
    x = 150;
    y = 150; 
    speedX = random(3, 5);
    speedY = random(3, 5);
    rectSize = 150;
  }

  // if ball hits up or down, change direction of Y   
  if ( y > height || y < 0 ) {
    speedY = speedY * -1;
    y = y + speedY;
  }
}


////////////////////////////////////////////////

Serial createSerial(String name)
{
  try
  {
    return new Serial(this, name, 9600);
  }
  catch(Exception e)
  {
    System.err.print(e.getMessage());
    return null;
  }
}

void serialEvent(Serial port)
{
  inString = port.readString();
}

void mousePressed()
{
  portSelect.clickHandler();
}

class ComPortMenuButton
{
  private int mX;
  private int mY;
  private int mWidth;
  private int mHeight;
  private String mName;
  private boolean mIsActive;

  public ComPortMenuButton(int btnX, int btnY, int btnWidth, int btnHeight, String btnName)
  {
    mX = btnX;
    mY = btnY;
    mWidth = btnWidth;
    mHeight = btnHeight;
    mName = btnName;
    mIsActive = false;
  }

  boolean mouseOverBtn()
  {
    return (mouseX>mX && mouseX<mX+mWidth && mouseY>mY && mouseY<mY+mHeight);
  }

  public void drawButton()
  {
    fill(70);
    if (mIsActive)
    {
      fill(120, 80, 0);
    }
    if (mouseOverBtn())
    {
      fill(180, 120, 0);
    }
    rect(mX, mY, mWidth, mHeight);
    fill(255);
    textAlign(CENTER);
    text(mName, mX+mWidth/2, mY+mHeight/2+8);
  }

  void setInActive()
  {
    mIsActive = false;
  }

  Serial setActive()
  {
    mIsActive = true;
    return createSerial(this.mName);
  }
}

class ComPortMenu
{
  private ArrayList<ComPortMenuButton> mButtons;
  private ComPortMenuButton mActiveBtn = null;
  private Serial mComPort = null;

  public ComPortMenu()
  {
    int i = 0;
    mButtons = new ArrayList();
    for (String current : Serial.list())
    {
      int x = i % 5;
      int y = i / 5;
      mButtons.add(new ComPortMenuButton(100*x+5, 50*y+5, 90, 40, current));
      i++;
    }
  }

  public void drawMenu()
  {
    for (ComPortMenuButton current : mButtons)
    {
      current.drawButton();
    }
  }

  public void clickHandler()
  {
    for (ComPortMenuButton current : mButtons)
    {
      if (current.mouseOverBtn())
      {
        if (mActiveBtn != null)
          mActiveBtn.setInActive();
        mActiveBtn = current;
        mComPort = current.setActive();
      }
    }
  }

  public Serial getComPort()
  {
    return mComPort;
  }
}
