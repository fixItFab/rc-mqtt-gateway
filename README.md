# RC MQTT Gateway

Receiving mqtt message and send rc commands based on [rc-switch](https://github.com/sui77/rc-switch)

Board: ESP8266

Command Format:

```JSON
{ code: 4259861, protocol: 1 }
```

## Usage

Rename the `credentials.sample.h` file to `credentials.h` and enter your wifi and mqtt data
