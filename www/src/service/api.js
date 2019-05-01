
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
    const data = await fetch('/config').then(response => response.json());
    console.log('response', data);
    //const data = response.json;
    data.wifi.forEach(net => (net._id = newId()));
    return data;
}

const post = async (url, data) => {
  return await fetch(url, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: data && JSON.stringify(data), // body data type must match "Content-Type" header
  })
};

export const save = async config => {
  console.log('SAVE THE CONFIG:', config);
  return await post('/config', config);
};

export const restart = async () => {
  // besides calling the API we need to wait for about 5-10 seconds
  await post('/restart');
  await wait(5000);
};


