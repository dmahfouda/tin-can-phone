# tin-can-phone

### Configuration
Before compiling with Arduino IDE, create a new file in the tin-can-phone directory called "Config.h"

Then copy the contents of Config.sample.h into Config.h 

```
#define TIN_CAN_SSID "<wifi-name>"
#define TIN_CAN_PASSWORD "<wifi-password>"
#define TIN_CAN_ID "<can-name>"
#define TIN_CAN_SWITCH "<local ip address>"
#define TIN_CAN_SWITCH_PORT "<port listed in tin>"
```

Update these values to suit your local environment.

In order to find your IP address

```
ifconfig
```

Then look for the ip address at 

```
en0:
inet
```

In order to find your port address, you need to check forwarding in three places, Virtual Box, docker.yaml (in tin-can-switch), and in the index.js file (also in tin-can-switch)
