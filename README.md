# LEDSpectrumAnalyzer

This project is a seven band spectrum analyzer used to drive a LED Matrix given an audio signal. The code is running on an Arduino Nano and the audio processing is done by an MSGEQ7 module, the board can be controlled via Bluetooth with a simple Android App.


https://user-images.githubusercontent.com/56454542/176924004-93b5298e-80a9-4b6a-b8a6-2eec0a4a9c40.mp4


## How does it work?

The audio input signal is passed to an MSGEQ7 module which has seven pass-band filters, the module outputs the amplitude for each band of frequencies. The arduino controls three different LED strips using FASTLed libraries: the corner, the center, the little DOT in the bottom of the frame. 

#### Corner led strip

The corner led strip ligths the led based on different animation designed to be visually captivating. The animations, as well as their colour and duration, are randomized and they are only changing under certain conditions (e.g average low frequencies read coming from MSGEQ7 >= 50%), this gives the frame a really dynamic transition between animations. A two-way reflective sheet was placed in both sides of the frame, giving the corner a cool "infinity mirror" effect.

#### Central led strip

Central led strip lights the LEDs based on MSGEQ7 output, every 40 ms the module updates its outputs, these levels of amplitude are passed to the arduino that maps the values in a range 0-10 and lights the leds in different colours based on the frequencies.

| Colour     | Frequency band |
| ---      | ---       |
| RED | 0Hz-63Hz |
| ORANGE | 63Hz-160Hz |
| YELLOW | 160Hz-400Hz |
| GREEN | 400Hz-1kHz |
| LIGHT BLUE | 1kHz-2.5kHz |
| BLUE | 2.5kHz-6.25kHz |
| PURPLE | 6.25kHz-16kHz |

#### DOT led strip

The dot in the bottom of the strip is used as a flag for bluetooth data, when data is being received it turns blue to show that the data was received correctly.

## Additional features

#### Controllable via App

I built a simple app using MitApp inventor to control the frame via smartphone. It is possible to connect to the board, turn the board ON/OFF, adjust brightness or choose a visualization mode. </br>
</br>
![Controller](https://user-images.githubusercontent.com/56454542/176923487-4343dd59-1eab-460a-8c5e-a7703836bc58.png)
</br> 
In Party mode both the corner and the center generates different animations as shown in the demo video. </br> 
In Static mode every led from corner and center is turned with an optional color (corner and center colours are different), the color for both the center and the corner can be controlled via app.</br>
In Relax mode the corner's leds are on in a still white colour, creating a more relaxing animation. </br> 
Strobo mode needs no introduction.

Another LED project i made of which i have a little introduction: 

https://user-images.githubusercontent.com/56454542/176926289-476595c1-1278-4245-91f6-26ae7a73be42.mp4

Animations i programmed and project's info:

https://youtu.be/t5UIrvfEN9Y?t=300




