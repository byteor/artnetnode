import { store } from 'preact-easy-state';

export default store({
    hw: {
        /** PWM frequency */
        freq: undefined,
        /** Module ID */
        id: undefined
    },
    /** Host name */
    host: undefined,
    wifi: [
        /*
        {
            ssid: undefined,
            pass: undefined,
            dhcp: undefined
        }
        */
    ],
    dmx: [
        /*{
          channel: undefined,
          type: undefined,
          pin: undefined,
          pulse: undefined,
          multiplier: undefined,
          threshold: undefined
        }*/
    ]
});