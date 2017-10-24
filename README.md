# Fochica™ - Forgotten Child in Car Alert

Fochica, forgotten child in car alert, is a no hassle, low cost and extensible system that helps prevent forgetting children in vehicles.

Kids who are left alone in cars are likely to suffer a heat stroke with dire consequences. People think this will never happen to them but in reality, cases like this happen over and over again every year. We use smart devices to wake us up in the morning, to remind us about meetings and TV shows we might miss. Why not use technology to make sure you don’t forget your kids? Fochica’s mission is to prevent forgetting children in vehicles and thus save children’s lives.

Fochica is comprised of a Fochica open hardware, open software, Arduino-based device that is installed in a car and a Fochica app that is installed on the driver’s smartphone. Sensors monitor whether the child car seats are occupied or empty and send the data to the device. The device communicates with the app via Bluetooth Low Energy passing the information to the driver. When the driver’s smartphone goes out of range and a seat is occupied an alarm will sound on the smartphone.

This is the source code repository of the Fochica device. For build instructions visit the [Fochica Wiki](https://github.com/fochica/fochica-wiki/wiki).

For more information see [fochica.com site](http://fochica.com) or the [hackaday page](https://hackaday.io/project/20902-fochica-forgotten-child-in-car-alert) .

[![Fochica](https://img.youtube.com/vi/8wOeXHNUfBQ/0.jpg)](https://www.youtube.com/watch?v=8wOeXHNUfBQ)

![Fochica NanoV1 prototypes](https://cdn.hackaday.io/images/910051506800531124.jpg)

![Fochica ProtoShield](http://fochica.com/wiki/protoshield/fochica%20protoshield%20with%20extras.jpg "Fochica ProtoShield")

![Device prototype](http://fochica.com/wp-content/uploads/2017/03/device-slide.jpg)

## Dependencies
* [RTClib](https://github.com/adafruit/RTClib)