//fetchMock.mock('http://example.com', 200);

export const getFoo = async () => {
  const res = await fetch('http://google.com');
  assert(res.ok);
  return res.json;
};

export const newId = () => {
  return (
    '_' +
    Math.random()
      .toString(36)
      .substr(2, 9)
  );
};

export const wait = async howLong => {
  return new Promise(resolve => setTimeout(resolve, howLong));
};

export const load = async () => {
  const data = {
    host: 'foo-bar',
    hw: { freq: 200 },
    wifi: [
      { ssid: 'ssid1', pwd: 'pwd1', dhcp: true },
      { ssid: 'ssid2', pwd: 'pwd2', dhcp: false, ip: '111.222.333.4', netmask: '', dns: '' },
    ],
    dmx: [
      {
        channel: 1,
        type: 1,
        pin: 2,
        pulse: 20,
        threshold: 127,
        multiplier: 5,
      },
      {
        channel: 1,
        type: 2,
        pin: 22,
        pulse: 20,
        threshold: 127,
        multiplier: 5,
      },
    ],
  };
  data.wifi.forEach(net => (net._id = newId()));
  await wait(1000); // TODO: remove the delay
  return data;
};

export const save = async config => {
  console.log('SAVE THE CONFIG:', config);
  await wait(1000); // TODO: remove the delay
};

export const restart = async () => {
  // besides calling the API we need to wait for about 5-10 seconds
  await wait(5000);
};
