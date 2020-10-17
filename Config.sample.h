/*
 * The fields below hold information relevant to
 * communiticating with a switching server.
 */
#define TIN_CAN_SWITCH_PROTOCOL "http"
#define TIN_CAN_SWITCH_HOST "<remote-ip>"
#define TIN_CAN_SWITCH_PORT "<remote-port>"
#define TIN_CAN_SWITCH_BOX "<box-name>"

/*
 * Number of seconds to wait before polling
 * the server for an updated box state.
 */
#define TIN_CAN_SWITCH_BOX_UPDATE_INTERVAL 5

/*
 * When the device is powered on, if it cannot
 * connect to a WiFi network, it will turn into
 * an access point. One can then connect to it using
 * the credentials below. Then, in a browser, go to
 * `192.168.1.1` and configure the appropriate fields.
 * 
 * The fields below hold the credentials for when the
 * device is acting as an access point.
 */
#define TIN_CAN_PHONE_ACCESS_POINT "TinCanPhone-AccessPoint"
#define TIN_CAN_PHONE_PASSWORD "password"

/*
 * The fields below configure the audio
 * settings for the device.
 */
#define TIN_CAN_PHONE_SAMPLE_RATE 8000
#define TIN_CAN_PHONE_EMA_ALPHA 0.3
#define TIN_CAN_PHONE_AUDIO_FILENAME "audio.pcm"
