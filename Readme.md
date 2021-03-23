## Usefull Resource
[dm00226326-using-the-hardware-realtime-clock-rtc-and-the-tamper-management-unit-tamp-with-stm32-microcontrollers-stmicroelectronics.pdf](https://www.st.com/resource/en/application_note/dm00226326-using-the-hardware-realtime-clock-rtc-and-the-tamper-management-unit-tamp-with-stm32-microcontrollers-stmicroelectronics.pdf)

## Useful IDE setting
* Go to 
``` 
Run -> External Tools -> External Tools Configuration 
```
Under Main Tab
* Add new configuration with below values
    
    * Name: Load Flash
    * Location: Path of STMFlashLoader.exe like ``` C:\Program Files (x86)\STMicroelectronics\Software\Flash Loader Demo\STMFlashLoader.exe ```
    * Working Directory: ``` ${workspace_loc:/plant-watering-system/Release} ```
    * Arguments: 
    ```
    -c --pn 3 --br 115200 --db 8 --pr EVEN --sb 1 --ec OFF --to 10000 --co OFF -i STM32F0_3x_32K -e --all -d --fn plant-watering-system.bin --a 08000000 --v --o
    ```
Under Refresh Tab
* Tick ```Refresh resources upon completion``` with opt ```The entire workspace```
* Tick ```Recursively include sub-folders```

Under Build Tab
* Tick ```Build before launch```

* Click Apply and close window
