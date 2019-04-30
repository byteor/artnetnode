import { store } from 'preact-easy-state';
import { restart, load, save, newId } from './api';

const espStore = store({
  isLoading: false,
  error: undefined,
  config: {
    hw: {
      /** PWM frequency */
      freq: 100,
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
