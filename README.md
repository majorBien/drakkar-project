Attention!!!! the entire project was implemented by the Hackathony "Aviatech Challange" in Mielec in time shorter than 24h

Thermal flight assistant
![drakkar](https://github.com/majorBien/drakkar-project/assets/122567282/e06a6866-3b9f-4182-b599-cbed8aefec30)



Authors:
Wiktor Bień - Embedded Developer
Jakub Konior - Backend Developer
Filip Krawczyk - Frontend Developer
Kamil Bęben - Airplane Subject Matter Expert
The Drakkar project, i.e. a device that facilitates thermal flights by integrating flight parameters measurements, communication between devices/users and ultimately a predictive system showing the areas of current and likely movements of air masses is also equipped with a device supporting the location of the thermal chimneys center.
Technologies that we've used:
![techs](https://github.com/majorBien/drakkar-project/assets/122567282/61d99629-9f00-40d0-965c-fbe497031654)

 
A system consisting of 2 displays cooperating with each other 
![concept](https://github.com/majorBien/drakkar-project/assets/122567282/4f82219e-fd74-4118-9807-29f00a2f07b4)

The main display showing the map on which "flags" are displayed, whose color means how much it was applied to by users marking detected thermal chimneys and predictive flags if the flag often appears in a given place and we have weather conditions similar to those when the flag is erected there, The flags additionally give us information about when it was placed, what was the average vertical speed, the height of the input and exit from the chimney, ultimately using topographic, historical and current weather data, and the constantly developing technology AI of the overlay showing the anticipated areas of air mass movements by showing probable wear, souls, etc.
Additional display showing our speed, height and arrows showing the likely center of the thermal chimney, the values are to choose from km/h and m and kt and ft altimeter setting with the possibility of regulating local pressure, speed tape showing our current speed with additional markers recommending flight at some speed Depending on the situation, e.g. a flight in Termice, a jump between chimneys, stewing area, and markers, recommending the flap setting depending on the speed, we use the converted barometric height thanks to the BME280 pressure sensor as well as speed and location from the GPS module.
The device will download data from the database whenever it has access to the Internet, so we consider the options for using miniature SatCom modules.

Front-end application documentation
https://github.com/fifcio13/DrakkarAviaTech
Author: Filip Krawczyk
Technologies
The Front-End application was built in JavaScript using the React Framework. The choice of this technology was dictated by the following factors:
The ease of creating standalone applications with the help of Electron: due to the saving of Raspberry Pi frames and technical restrictions resulting from the low version of the equipment.
Easy integration with Google maps: ease of adding planned functionalities.
The speed of writing in React: facilitates rapid development.
Simplified connections to the database (MySQL): easier data management.
In addition to standard REACT and Electron libraries, we also use the @vis.gl/react-google-maps library, which allows a faster and more accessible editing of map components. We connect to Google maps remotely with Google API Console.
At the current stage of work, we do not use the full reactivity, but this allows us to expand and adapt the application according to the needs of users. Unfortunately, due to the Raspberry Pi 3, we are not able to fully use the potential of maps due to the too weak graphic processor. We are limited to using raster maps instead of more extensive and possible to adapt vector maps. (No support for webgpu)


Operation of the application
The application works as follows:
Preload: When charging, the application makes a connection to the database and Google maps.
Data charging: Map tags loaded from the JSON file and user location are applied.
Map view: After loading the map, the view with the map is launched. After clicking, tags display their detailed description. The color and intensity (transparency) of the marker mean its "freshness":
Red: The most current (<30 min)
Orange: medium current (<90 min)
Green translucent: less current (<5 h)
Markers over 5 hours do not display.
Markers are refreshed if the user reports that the thermal chimney is still occurring.
Development plans
Ultimately, the application will not have hardware restrictions due to the choice of other equipment, which will allow further expansion and improvement of current functions. First of all, there are plans:
Predicture and expert system - i.e. an overlay thanks to which the user has the opportunity to see predictions as to the conditions in the air.
Improving map rotation.
Greater control over markers.
Possibility to turn off some information on maps: e.g. displaying store markers, parks.
Improving the fluidity of the map.

![app_ss](https://github.com/majorBien/drakkar-project/assets/122567282/11824d8f-2fe1-4e40-9cc8-7cfd5823cd76)

![app_ss2](https://github.com/majorBien/drakkar-project/assets/122567282/7837cb90-1188-4bb0-a863-71cb1346ea1f)

Embedded System Documentation
https://github.com/majorbien/drakkar-project
Author: Wiktor Bień
Technologies

C
Freertos
ESP IDF Framework
The Drakkar project was created by using advanced technologies that ensure its reliability and performance. The built -in system based on ESP32 was programmed in C language, using the ESP ISP Framework, which allowed for effective use of the capabilities of this microcontroller. ESP32 offers not only high computing power, but also a wide range of communication interfaces, which was crucial for the integration of various functions in the Drakkar device.
In addition, the use of Freertos as a light real time operating system adds a layer of stability and flexibility to the system by the possibility of multi -threaded programming. Thanks to Freertos, the Drakkar device can perform many tasks at the same time, which is extremely important for air/avionic systems. Freertos enables precise task management, which is crucial to ensure the fluidity of the device even in dynamic flight conditions.
Thanks to this combination of advanced technologies, the Drakkar project is not only an innovative solution for glider community, but also an extremely stable and versatile product, ready to be used in various aerial conditions.




System operation
The application works as follows:
The application collects data from various sensors, each of which is operated by a separate thread, which allows you to read and process data from many sources at the same time. For example, data from height sensors, vertical speed, acceleometer and gyroscope are collected in separate threads, which ensures smooth operation of the application even at intensive load.
The buttons support are also carried out in a separate thread, which allows the reaction to the lack of disruptions of various -class processes.
To calculate the height above sea level (NPM), the application uses the Calculatealtitude function, which is based on atmospheric pressure measurements and pressure at sea level. Using these parameters and physical constant, this function makes calculations based on the atmospheric model to return the exact value of the height above sea level.
However, the UpdateverticalVelocity function is used to update vertical speed based on a gyroscope data. Depending on the settings, it can return the speed in meters per second or in the feet per minute. (Fig A)

![image](https://github.com/majorBien/drakkar-project/assets/122567282/33cccdd0-d869-4bac-b1c5-da39f5f0f004)

The detection of thermal chimneys is done by analyzing differences in medium heights on the left and right side of the glider. If this difference exceeds the set threshold, the application recognizes that we are in the thermal chimney. Then, based on the comparison of medium -heights, the application determines the direction in which to turn to use the thermal chimney. If the average heights are similar, the direction is considered indefinite, which means no thermal chimney within reach. Based on these calculations, the application provides the recommended direction to control the glider, which allows effective use of thermal currents during the flight. This is visualized by a green indicator. 


![m51](https://github.com/majorBien/drakkar-project/assets/122567282/2b249c62-20f5-470b-bf0e-1296b5863449)



6. Sensors and display support have been made on a separate core than the avionics operation, which gives greater efficiency of the system.

Schematic diagram

![Schematic_aviatech_2024-05-19](https://github.com/majorBien/drakkar-project/assets/122567282/bd79581c-b127-4602-a82c-84c190d5c2fd)



As you can see in the electronic scheme, we are dealing with the integration of two devices constituting one cooperating system: ESP32 and Raspberry Pi Zero. ESP32 is equipped with a 2-inch display while Raspberry can be connected to any HDMI display/monitor. ESP32 and Rhapsbeers communicate using the UART protocol to the transmission parameters below:
         .baud_rate = 9600,
         .data_bits = UART_DATA_8_BITS,
         .Parity = uart_parity_disable,
         .stop_bits = uart_stop_bits_1,
         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE

Development plans
1. Extending functionality: Continuation of working on the extension of system functionality by adding new sensors and integration of additional communication modules. For example, you can consider adding Lora data transmission module, which would allow you to monitor the location of the glider and communication over higher distances.

2. Efficiency optimization: constant system code optimization and optimization of threads to minimize the consumption of hardware resources and energy. Improving performance can be achieved by optimizing data processing algorithms and reduction of delays in the system.

3. Development of the user interface: Improving the user interface, both in terms of aesthetics and usability. The ability to personalize the interface and add new functions, such as displaying the map with selected thermal areas or the ability to save flight routes, can significantly increase the attractiveness of the system for users.


6. Tests and implementation: continuation of system tests in various aerial conditions and further improving data analysis algorithms. After the tests are completed, subsequent system implementation is planned on a larger scale, both among glider enthusiasts and in professional air schools.

7. Graphic indications of the optimal speed for flights during stewing and wearing- i.e. the overlay thanks to which the user will have the opportunity to see predictions as to the conditions in the air.
8. Indications of the optimal flap position for a given point of flight.
9. Adding a glider selection interface with its data such as perfection, mass, wing span and flap positions for greater functionality of the system.

10. Designing the PCB system

   ![m52](https://github.com/majorBien/drakkar-project/assets/122567282/a32394b0-1099-4c76-8320-0154f7437f40)

   ![C998754E-7A59-4C72-B53F-6C33FCC32C3E](https://github.com/majorBien/drakkar-project/assets/122567282/53d3d50d-6f21-438a-b6c8-186f9d272dd1)



Raspberry Pi backend application repository
https://github.com/grappas/json_parser_aviatech
Author: Jakub Konior

