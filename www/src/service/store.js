import { store } from 'preact-easy-state';

const newId = () => {
  return (
    '_' +
    Math.random()
      .toString(36)
      .substr(2, 9)
  );
};

const load = async () => {
  const data = {
    host: 'foo-bar',
    wifi: [
      { ssid: 'ssid1', pwd: 'pwd1', dhcp: true },
      { ssid: 'ssid2', pwd: 'pwd2', dhcp: false, ip: '111.222.333.4', netmask: '', dns: '' },
    ],
  };
  data.wifi.forEach(net => net._id = newId());
  return data;
};

const save = async config => {
  console.log('SAVE THE CONFIG', config);
};

const restart = async () => {
  // besides calling the API we need to wait for about 5-10 seconds
  return new Promise(resolve => setTimeout(resolve, 5000));
};

const espStore = store({
  isLoading: true,
  error: undefined,
  config: {
    hw: {
      /** PWM frequency */
      freq: undefined,
      /** Module ID */
      id: undefined,
    },
    /** Host name */
    host: undefined,
    wifi: [],
    dmx: [],
  },
  newId: newId,
  async load() {
    espStore.isLoading = true;
    try {
      espStore.config = await load();
    } catch (e) {
      espStore.error = e;
      console.log(e);
    } finally {
      espStore.isLoading = false;
    }
  },
  async save() {
    try {
      espStore.isLoading = true;
      await save(espStore.config);
    } catch (e) {
      espStore.error = e;
      console.log(e);
    } finally {
      espStore.isLoading = false;
    }
  },
  async restart() {
    try {
      espStore.isLoading = true;
      await restart();
      await load();
    } catch (e) {
      espStore.error = e;
      console.log(e);
    } finally {
      espStore.isLoading = false;
    }
  },
});

export default espStore;
